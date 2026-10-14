#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "motor.h"
#include "line.h"
#include "reader.h"
#include "utils.h"
#include "capteur_color.h"
#include "buzzer.h"
#include "mon_lcd.h"

// --- constantes ---
#define SPEED_NORMAL 50
#define SPEED_TURN 30
#define SPEED_CORRECTION_OFFSET 15
#define SPEED_SEARCH 20

#define DELAY_SHORT_US 10000       // 10ms
#define DELAY_STOP_US 100000      // 100ms
#define DELAY_FORWARD_US 200000  // 200ms

// --- variables globales ---
volatile int current_detected_color = 0;
volatile bool system_running = true;
char last_lcd_message[32] = "";
pthread_t color_thread_id;

// --- fonctions auxiliaires ---

void update_lcd_safe(const char* message) {
    if (strncmp(last_lcd_message, message, 31) != 0) {
        lcd_afficher(message);
        strncpy(last_lcd_message, message, 31);
        last_lcd_message[31] = '\0';
    }
}

void handle_sigint(int sig) {
    printf("\nSignal reçu %d. Arrêt du robot...\n", sig);
    system_running = false;
    motor_stop();
    pthread_join(color_thread_id, NULL);
    exit(0);
}

void* color_check_thread(void* arg) {
    int color_fd = *((int*)arg);
    int last_color = 0;
    
    while (system_running) {
        int c = lire_couleur(color_fd);
        current_detected_color = c;
        
        // Bip pour vert et rouge
        if ((c == 1 || c == 2) && c != last_color) {
            buzzer_beep(50);
        }
        
        last_color = c;
        usleep(DELAY_SHORT_US);
    }
    return NULL;
}

/**
 * Suivi de ligne simple
 * Retourne true si la ligne est détectée, false sinon
 */
bool follow_line_step(const char* lcd_msg) {
    bool left = line_front_left();
    bool right = line_front_right();
    bool center = line_front_center();

    if (center) {
        motor_set_left_speed(SPEED_NORMAL);
        motor_set_right_speed(SPEED_NORMAL);
        if (lcd_msg) update_lcd_safe(lcd_msg);
        return true;
    } else if (left) {
        motor_set_left_speed(SPEED_NORMAL - SPEED_CORRECTION_OFFSET);
        motor_set_right_speed(SPEED_NORMAL + SPEED_CORRECTION_OFFSET);
        if (lcd_msg) update_lcd_safe("CORRECTION DRT");
        return true;
    } else if (right) {
        motor_set_left_speed(SPEED_NORMAL + SPEED_CORRECTION_OFFSET);
        motor_set_right_speed(SPEED_NORMAL - SPEED_CORRECTION_OFFSET);
        if (lcd_msg) update_lcd_safe("CORRECTION GCH");
        return true;
    } else {
        motor_set_left_speed(SPEED_SEARCH);
        motor_set_right_speed(SPEED_SEARCH);
        if (lcd_msg) update_lcd_safe("CHERCHE LIGNE");
        return false;
    }
}

/**
 * Commande TG ou TD : Tourne à gauche ou à droite (NE FAIT QUE TOURNER)
 */
void cmd_turn(Command direction) {
    if (direction != LEFT && direction != RIGHT) return;

    const char* dir_str = (direction == LEFT) ? "GAUCHE (TG)" : "DROITE (TD)";
    printf("=== Exécution: Tourner %s ===\n", dir_str);
    update_lcd_safe((direction == LEFT) ? "VIRAGE GAUCHE" : "VIRAGE DROITE");

    // Définir les vitesses de rotation
    if (direction == LEFT) {
        motor_set_left_speed(-SPEED_TURN);
        motor_set_right_speed(SPEED_TURN);
    } else {
        motor_set_left_speed(SPEED_TURN);
        motor_set_right_speed(-SPEED_TURN);
    }
    
    // Étape 1: Tourner jusqu'à quitter la ligne
    bool on_line = true;
    while (on_line && system_running) {
        if (!line_front_left() && !line_front_center() && !line_front_right()) {
            on_line = false;
        }
        usleep(DELAY_SHORT_US);
    }
    
    printf("  Ligne perdue, recherche nouvelle orientation...\n");

    // Étape 2: Continuer jusqu'à retrouver la ligne
    bool found_line = false;
    while (!found_line && system_running) {
        if (line_front_left() || line_front_center() || line_front_right()) {
            found_line = true;
        }
        usleep(DELAY_SHORT_US);
    }
    
    motor_stop();
    usleep(DELAY_STOP_US);
    printf("  Virage terminé, robot réorienté.\n");
}

/**
 * Commande AV : Avancer jusqu'au prochain croisement
 */
void cmd_forward() {
    printf("=== Exécution: Avancer (AV) ===\n");
    update_lcd_safe("AVANCE (AV)");
    
    // Suivre la ligne jusqu'à détecter une intersection
    bool intersection_detected = false;
    
    while (!intersection_detected && system_running) {
        // Vérifier les capteurs arrière (détection d'intersection)
        bool back_left = line_back_left();
        bool back_right = line_back_right();
        
        if (back_left || back_right) {
            printf("  Intersection détectée (BL:%d, BR:%d)\n", back_left, back_right);
            intersection_detected = true;
            break;
        }
        
        // Continuer à suivre la ligne
        follow_line_step("AVANCE (AV)");
        usleep(DELAY_SHORT_US);
    }
    
    motor_stop();
    usleep(DELAY_STOP_US);
    printf("  Robot positionné à l'intersection.\n");
}

/**
 * Vérifie si on a atteint la zone d'arrivée (rouge après avoir quitté le départ)
 */
bool check_arrival(bool has_left_start) {
    if (current_detected_color == 2 && has_left_start) {
        printf("\n===========================================\n");
        printf("  ARRIVÉE DÉTECTÉE (ZONE ROUGE) - FIN\n");
        printf("===========================================\n");
        update_lcd_safe("ARRIVEE!");
        motor_stop();
        buzzer_beep(200);
        usleep(200000);
        buzzer_beep(200);
        return true;
    }
    return false;
}

int main() {
    signal(SIGINT, handle_sigint);

    char* input = readStdinDynamic();
    if (!input) {
        fprintf(stderr, "Erreur : impossible de lire les commandes\n");
        return 1;
    }

    CommandList* commands = read_command(input);
    if (!commands || commands->size == 0) {
        fprintf(stderr, "Erreur : liste de commandes invalide\n");
        free(input);
        return 1;
    }
    
    printf("Commandes chargées : %d\n", commands->size);
    printCommand(commands);

    if (motor_init() != 0) {
        fprintf(stderr, "Erreur : initialisation moteurs\n");
        freeCommandList(commands);
        free(input);
        return 1;
    }
    
    if (line_init() != 0) {
        fprintf(stderr, "Erreur : initialisation capteurs ligne\n");
        freeCommandList(commands);
        free(input);
        return 1;
    }
    
    buzzer_setup();
    lcd_initialisation();
    lcd_effacer();
    update_lcd_safe("INIT OK");
    
    int color_sensor_fd = color_initialisation();
    if (color_sensor_fd < 0) {
        fprintf(stderr, "Avertissement : capteur couleur non initialisé\n");
    }

    if (pthread_create(&color_thread_id, NULL, color_check_thread, &color_sensor_fd) != 0) {
        fprintf(stderr, "Erreur : création thread couleur\n");
    }
    
    printf("Robot prêt.\n");
    update_lcd_safe("DEPART!");
    usleep(500000);  // Pause 500ms

    bool has_left_start_zone = false;
    for (int cmd_idx = 0; cmd_idx < commands->size && system_running; cmd_idx++) {
        if (current_detected_color != 2 && !has_left_start_zone) {
            has_left_start_zone = true;
            printf("Zone de départ quittée.\n\n");
        }
        
        if (check_arrival(has_left_start_zone)) {
            system_running = false;
            break;
        }
        
        Command cmd = commands->list[cmd_idx];
        printf(">>> Commande %d/%d <<<\n", cmd_idx + 1, commands->size);
        
        switch (cmd) {
            case LEFT:
                cmd_turn(LEFT);
                break;
                
            case RIGHT:
                cmd_turn(RIGHT);
                break;
                
            case FORWARD:
                cmd_forward();
                break;
                
            default:
                printf("ERREUR : Commande inconnue\n");
                update_lcd_safe("CMD INCONNUE");
                system_running = false;
                break;
        }
        
        printf("\n");
        
        if (check_arrival(has_left_start_zone)) {
            system_running = false;
            break;
        }
    }
    
    if (system_running) {
        printf("Toutes les commandes exécutées.\n");
        printf("Suivi de ligne jusqu'à l'arrivée...\n");
        update_lcd_safe("VERS ARRIVEE");
        
        while (system_running) {
            if (check_arrival(has_left_start_zone)) {
                break;
            }
            
            follow_line_step("VERS ARRIVEE");
            usleep(DELAY_SHORT_US);
        }
    }
    
    printf("Arrêt du robot...\n");
    system_running = false;
    motor_stop();
    pthread_join(color_thread_id, NULL);

    freeCommandList(commands);
    free(input);
    
    printf("Programme terminé\n");
    return 0;
}
