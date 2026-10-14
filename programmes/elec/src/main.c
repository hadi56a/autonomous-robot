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
#define SPEED_NORMAL 30
#define SPEED_TURN 20
#define SPEED_CORRECTION_OFFSET 10
#define SPEED_SEARCH 20

#define DELAY_SHORT_US 10000       // 10ms
#define DELAY_STOP_US 100000      // 100ms
#define DELAY_FORWARD_US 200000  // 200ms

// --- variables globales ---
pthread_mutex_t lcd_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile bool color_priority_mode = false;
volatile int current_detected_color = 0;
volatile bool system_running = true;
char last_line1[17] = "                ";
char last_line2[17] = "                ";
pthread_t color_thread_id;

// --- fonctions auxiliaires ---

void update_lcd_dual(const char* l1, const char* l2) {
    if (color_priority_mode) return; 

    pthread_mutex_lock(&lcd_mutex); 

    char buf1[17];
    char buf2[17];

    snprintf(buf1, 17, "%-16s", l1 ? l1 : "");
    snprintf(buf2, 17, "%-16s", l2 ? l2 : "");

    if (strcmp(last_line1, buf1) != 0) {
        lcd_afficher_xy(0, 0, buf1);
        strncpy(last_line1, buf1, 16);
        last_line1[16] = '\0';
    }
    if (strcmp(last_line2, buf2) != 0) {
        lcd_afficher_xy(0, 1, buf2);
        strncpy(last_line2, buf2, 16);
        last_line2[16] = '\0';
    }

    pthread_mutex_unlock(&lcd_mutex);
}

void update_lcd_safe(const char* message) {
    update_lcd_dual(message, "");
}

void force_lcd_update_protected(const char* l1, const char* l2) {
    pthread_mutex_lock(&lcd_mutex);
    
    char buf1[17];
    char buf2[17];
    snprintf(buf1, 17, "%-16s", l1);
    snprintf(buf2, 17, "%-16s", l2);
    
    lcd_afficher_xy(0, 0, buf1);
    lcd_afficher_xy(0, 1, buf2);
    
    strncpy(last_line1, buf1, 16); last_line1[16] = '\0';
    strncpy(last_line2, buf2, 16); last_line2[16] = '\0';
    
    pthread_mutex_unlock(&lcd_mutex);
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
        
        if ((c == 1 || c == 2) && c != last_color) {
            printf("Couleur detectee: %d\n", c);
            
            color_priority_mode = true; 
            
            if (c == 1) {
                force_lcd_update_protected("COULEUR", "VERTE !!!");
            } else {
                force_lcd_update_protected("COULEUR", "ROUGE !!!");
            }

            printf("BIIIIIIPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n");
            buzzer_beep(1500);

            usleep(1500000); 
            
            color_priority_mode = false;
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
bool follow_line_step(const char* main_instruction) {
    bool left = line_front_left();
    bool right = line_front_right();
    bool center = line_front_center();

    if (center) {
        motor_set_left_speed(SPEED_NORMAL);
        motor_set_right_speed(SPEED_NORMAL);
        update_lcd_dual(main_instruction, "TOUT DROIT");
        return true;
    } else if (left) {
        motor_set_left_speed(SPEED_NORMAL - SPEED_CORRECTION_OFFSET);
        motor_set_right_speed(SPEED_NORMAL + SPEED_CORRECTION_OFFSET);
        update_lcd_dual(main_instruction, "CORRECTION DRT");
        return true;
    } else if (right) {
        motor_set_left_speed(SPEED_NORMAL + SPEED_CORRECTION_OFFSET);
        motor_set_right_speed(SPEED_NORMAL - SPEED_CORRECTION_OFFSET);
        update_lcd_dual(main_instruction, "CORRECTION GCH");
        return true;
    } else {
        motor_set_left_speed(SPEED_SEARCH);
        motor_set_right_speed(SPEED_SEARCH);
        update_lcd_dual(main_instruction, "CHERCHE LIGNE");
        return false;
    }
}

/**
 * Commande TG ou TD : Tourne à gauche ou à droite (NE FAIT QUE TOURNER)
 */
void cmd_turn(Command direction) {
    // 1 = gauche, 2 = droite
    if (direction != 1 && direction != 2) return;

    printf("*** Starting turn... (Direction: %d)\n", direction);

    int speed = SPEED_TURN;
    
    if (direction == 1) { // gauche
        motor_set_left_speed(-10);
        motor_set_right_speed(speed);
    } else { // droite
        motor_set_left_speed(speed);
        motor_set_right_speed(-10);
    }
    // 0.6s
    usleep(600000);
    printf("fin du sleep\n");

    bool found_line = false;
    if (direction == 1) { // gauche
        motor_set_left_speed(-10);
        motor_set_right_speed(speed);
    } else { // droite
        motor_set_left_speed(speed);
        motor_set_right_speed(-10);
    }
    while (!found_line) {
        if (line_front_left() || line_front_right()) {
            printf("capteurs sur les cotes => stop\n");
            if (direction == 1) { // gauche
                motor_set_left_speed(0);
                motor_set_right_speed(15);
            } else { // droite
                motor_set_left_speed(15);
                motor_set_right_speed(0);
            }
        } else if (line_front_center()) {
            printf("capteur sur le centre => brake\n");
            motor_brake(15);
            found_line = true;
        }

        update_lcd_dual("TOURNE...", found_line ? "LIGNE TROUVEE" : "CHERCHE...");
        usleep(20000);
    }
    
    motor_brake(15);
    update_lcd_dual("TOURNE", "TERMINE");
    printf("virage complet.\n");
}

/**
 * Commande AV : Avancer jusqu'au prochain croisement
 */
void cmd_forward() {
    printf("=== Exécution: Avancer (AV) ===\n");
    update_lcd_dual("AVANCE", "INITIALISATION");
    
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
        follow_line_step("AVANCE");
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
    printf("check_arrival\n");
    printf("current_detected_color: %d\n", current_detected_color);
    printf("has_left_start: %d\n", has_left_start);
    if (current_detected_color == 2 && has_left_start) {
        printf("\n===========================================\n");
        printf("  ARRIVÉE DÉTECTÉE (ZONE ROUGE) - FIN\n");
        printf("===========================================\n");
        update_lcd_dual("ARRIVEE!", "STOP");
        motor_stop();
        buzzer_beep(200);
        usleep(200000);
        buzzer_beep(200);
        usleep(200000);
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
        
        Command cmd = commands->list[cmd_idx];
        printf(">>> Commande %d/%d <<<\n", cmd_idx + 1, commands->size);
        buzzer_beep(30);

        //usleep(30000);
        switch (cmd) {
            case LEFT:
                cmd_turn(LEFT);
                break;
                
            case RIGHT:
                cmd_turn(RIGHT);
                break;
                
            case FORWARD:
                if (cmd_idx != commands->size - 1) {
                    cmd_forward();
                }
                break;
                
            default:
                printf("ERREUR : Commande inconnue\n");
                update_lcd_safe("CMD INCONNUE");
                system_running = false;
                break;
        }
        
        printf("\n");
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
    
    pthread_mutex_destroy(&lcd_mutex); // <--- AJOUTER CECI
    
    printf("Programme terminé\n");
    return 0;
}
