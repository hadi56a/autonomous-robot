/**
 * @file mon_lcd.h
 * @brief Gestion de l'écran LCD via I2C
 * @details Ce module utilise un adaptateur PCF8574 pour contrôler un écran LCD via le bus I2C
 */

#ifndef MON_LCD_H
#define MON_LCD_H

/**
 * @brief Initialise l'écran LCD
 * @details Configure WiringPi, le PCF8574 et initialise l'écran LCD avec les paramètres par défaut (2 lignes, 16 caractères)
 * Active également le rétroéclairage
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int lcd_initialisation();

/**
 * @brief Affiche un message sur l'écran LCD
 * @details Efface l'écran avant d'afficher le message à la position (0,0)
 * @param message La chaîne de caractères à afficher
 */
void lcd_afficher(const char *message);

/**
 * @brief Affiche un message à une position spécifique sur l'écran LCD
 * @param x La colonne (0-15)
 * @param y La ligne (0-1)
 * @param message La chaîne de caractères à afficher
 */
void lcd_afficher_xy(int x, int y, const char *message);

/**
 * @brief Efface le contenu de l'écran LCD
 */
void lcd_effacer();

#endif