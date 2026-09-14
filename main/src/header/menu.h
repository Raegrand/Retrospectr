#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

enum MenuChoice {
  MENU_GALLERY,
  MENU_BIBLE,
  MENU_SETTINGS
};

// Launches the main menu and returns the selected choice
MenuChoice openMainMenu();
String openLanguageMenu();
int openTestamentMenu();
// Interactive submenu to adjust Text Speed & Sleep Timeout
void openSettingsSubmenu();
String openBibleBrowser(String currentPath, int testamentFilter);
#endif