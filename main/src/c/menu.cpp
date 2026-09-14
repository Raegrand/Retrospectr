#include "../header/menu.h"
#include "../header/display.h"
#include "../header/encoder.h"
#include "../header/power.h"
#include "../header/config.h"
#include <vector>
#include <algorithm>
#include <cctype>
#include <SD.h>  // FIX: Added to recognize the 'File' object
#include <SPI.h>


#ifndef ST77XX_DARKGREY
#define ST77XX_DARKGREY 0x52AA
#endif

struct FileItem {
  String name;
  bool isDir;
};

static const char* mainMenuItems[] = {
  "Gallery",
  "Bible",
  "Settings"
};
static const int TOTAL_MAIN_ITEMS = 3;

// BILINGUAL CHRONOLOGY DICTIONARY (English & Indonesia)
int getBibleBookOrder(String name) {
  name.replace('_', ' ');
  name.toLowerCase();

  // EPISTLES (Must check numbers first, e.g., "1 john" before "john")
  if (name.indexOf("1 john") >= 0 || name.indexOf("1 yohanes") >= 0) return 62;
  if (name.indexOf("2 john") >= 0 || name.indexOf("2 yohanes") >= 0) return 63;
  if (name.indexOf("3 john") >= 0 || name.indexOf("3 yohanes") >= 0) return 64;
  if (name.indexOf("1 samuel") >= 0) return 9;
  if (name.indexOf("2 samuel") >= 0) return 10;
  if (name.indexOf("1 kings") >= 0 || name.indexOf("1 raja") >= 0) return 11;
  if (name.indexOf("2 kings") >= 0 || name.indexOf("2 raja") >= 0) return 12;
  if (name.indexOf("1 chronicles") >= 0 || name.indexOf("1 tawarikh") >= 0) return 13;
  if (name.indexOf("2 chronicles") >= 0 || name.indexOf("2 tawarikh") >= 0) return 14;
  if (name.indexOf("1 corinthians") >= 0 || name.indexOf("1 korintus") >= 0) return 46;
  if (name.indexOf("2 corinthians") >= 0 || name.indexOf("2 korintus") >= 0) return 47;
  if (name.indexOf("1 thessalonians") >= 0 || name.indexOf("1 tesalonika") >= 0) return 52;
  if (name.indexOf("2 thessalonians") >= 0 || name.indexOf("2 tesalonika") >= 0) return 53;
  if (name.indexOf("1 timothy") >= 0 || name.indexOf("1 timotius") >= 0) return 54;
  if (name.indexOf("2 timothy") >= 0 || name.indexOf("2 timotius") >= 0) return 55;
  if (name.indexOf("1 peter") >= 0 || name.indexOf("1 petrus") >= 0) return 60;
  if (name.indexOf("2 peter") >= 0 || name.indexOf("2 petrus") >= 0) return 61;

  // OLD TESTAMENT (1-39)
  if (name.indexOf("genesis") >= 0 || name.indexOf("kejadian") >= 0) return 1;
  if (name.indexOf("exodus") >= 0 || name.indexOf("keluaran") >= 0) return 2;
  if (name.indexOf("leviticus") >= 0 || name.indexOf("imamat") >= 0) return 3;
  if (name.indexOf("numbers") >= 0 || name.indexOf("bilangan") >= 0) return 4;
  if (name.indexOf("deuteronomy") >= 0 || name.indexOf("ulangan") >= 0) return 5;
  if (name.indexOf("joshua") >= 0 || name.indexOf("yosua") >= 0) return 6;
  if (name.indexOf("judges") >= 0 || name.indexOf("hakim") >= 0) return 7;
  if (name.indexOf("ruth") >= 0 || name.indexOf("rut") >= 0) return 8;
  if (name.indexOf("ezra") >= 0) return 15;
  if (name.indexOf("nehemiah") >= 0 || name.indexOf("nehemia") >= 0) return 16;
  if (name.indexOf("esther") >= 0 || name.indexOf("ester") >= 0) return 17;
  if (name.indexOf("job") >= 0 || name.indexOf("ayub") >= 0) return 18;
  if (name.indexOf("psalm") >= 0 || name.indexOf("mazmur") >= 0) return 19;
  if (name.indexOf("proverbs") >= 0 || name.indexOf("amsal") >= 0) return 20;
  if (name.indexOf("ecclesiastes") >= 0 || name.indexOf("pengkhotbah") >= 0) return 21;
  if (name.indexOf("song") >= 0 || name.indexOf("kidung") >= 0) return 22;
  if (name.indexOf("isaiah") >= 0 || name.indexOf("yesaya") >= 0) return 23;
  if (name.indexOf("jeremiah") >= 0 || name.indexOf("yeremia") >= 0) return 24;
  if (name.indexOf("lamentations") >= 0 || name.indexOf("ratapan") >= 0) return 25;
  if (name.indexOf("ezekiel") >= 0 || name.indexOf("yehezkiel") >= 0) return 26;
  if (name.indexOf("daniel") >= 0) return 27;
  if (name.indexOf("hosea") >= 0) return 28;
  if (name.indexOf("joel") >= 0 || name.indexOf("yoel") >= 0) return 29;
  if (name.indexOf("amos") >= 0) return 30;
  if (name.indexOf("obadiah") >= 0 || name.indexOf("obaja") >= 0) return 31;
  if (name.indexOf("jonah") >= 0 || name.indexOf("yunus") >= 0) return 32;
  if (name.indexOf("micah") >= 0 || name.indexOf("mikha") >= 0) return 33;
  if (name.indexOf("nahum") >= 0) return 34;
  if (name.indexOf("habakkuk") >= 0 || name.indexOf("habakuk") >= 0) return 35;
  if (name.indexOf("zephaniah") >= 0 || name.indexOf("zefanya") >= 0) return 36;
  if (name.indexOf("haggai") >= 0) return 37;
  if (name.indexOf("zechariah") >= 0 || name.indexOf("zakharia") >= 0) return 38;
  if (name.indexOf("malachi") >= 0 || name.indexOf("maleakhi") >= 0) return 39;

  // NEW TESTAMENT (40-66)
  if (name.indexOf("matthew") >= 0 || name.indexOf("matius") >= 0) return 40;
  if (name.indexOf("mark") >= 0 || name.indexOf("markus") >= 0) return 41;
  if (name.indexOf("luke") >= 0 || name.indexOf("lukas") >= 0) return 42;
  if (name.indexOf("john") >= 0 || name.indexOf("yohanes") >= 0) return 43; 
  if (name.indexOf("acts") >= 0 || name.indexOf("kisah") >= 0) return 44;
  if (name.indexOf("romans") >= 0 || name.indexOf("roma") >= 0) return 45;
  if (name.indexOf("galatians") >= 0 || name.indexOf("galatia") >= 0) return 48;
  if (name.indexOf("ephesians") >= 0 || name.indexOf("efesus") >= 0) return 49;
  if (name.indexOf("philippians") >= 0 || name.indexOf("filipi") >= 0) return 50;
  if (name.indexOf("colossians") >= 0 || name.indexOf("kolose") >= 0) return 51;
  if (name.indexOf("titus") >= 0) return 56;
  if (name.indexOf("philemon") >= 0 || name.indexOf("filemon") >= 0) return 57;
  if (name.indexOf("hebrews") >= 0 || name.indexOf("ibrani") >= 0) return 58;
  if (name.indexOf("james") >= 0 || name.indexOf("yakobus") >= 0) return 59;
  if (name.indexOf("jude") >= 0 || name.indexOf("yudas") >= 0) return 65;
  if (name.indexOf("revelation") >= 0 || name.indexOf("wahyu") >= 0) return 66;

  return 999;
}

int extractNumber(const String& str) {
  String numStr = "";
  for (size_t i = 0; i < str.length(); i++) {
    if (isdigit(str[i])) numStr += str[i];
    else if (numStr.length() > 0) break; 
  }
  return numStr.length() > 0 ? numStr.toInt() : 0;
}

// --- RENDER MAIN MENU WITH INVERTED SELECTION ---
static void renderMainMenu(int selectedIndex) {
  tft.fillScreen(ST77XX_BLACK);

  // Decorative Header Title
  tft.setFont();
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(65, 15);
  tft.print("--- MENU ---");

  int boxWidth = 220;
  int boxHeight = 44;
  int startX = (TFT_WIDTH - boxWidth) / 2; // Centered horizontally (X = 30)
  int startY = 55;
  int gapY = 54;

  for (int i = 0; i < TOTAL_MAIN_ITEMS; i++) {
    int itemY = startY + (i * gapY);

    if (i == selectedIndex) {
      // SELECTED: Inverted block (Solid White Rectangle, Black Text)
      tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_WHITE);
      tft.setTextColor(ST77XX_BLACK);
    } else {
      // UNSELECTED: White text on Black background
      tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_BLACK);
      tft.drawRect(startX, itemY, boxWidth, boxHeight, ST77XX_DARKGREY); // Subtle outline
      tft.setTextColor(ST77XX_WHITE);
    }

    tft.setTextSize(2);
    
    // Calculate centered text position inside the option box
    int textX = startX + 25;
    int textY = itemY + 14;

    tft.setCursor(textX, textY);
    tft.print(mainMenuItems[i]);
  }
}

MenuChoice openMainMenu() {
  int selectedIndex = 0;
  renderMainMenu(selectedIndex);

  while (true) {
    // 1. Encoder Scroll Navigation
    int scroll = getEncoderScroll();
    if (scroll != 0) {
      selectedIndex += scroll;
      if (selectedIndex < 0) selectedIndex = TOTAL_MAIN_ITEMS - 1;
      if (selectedIndex >= TOTAL_MAIN_ITEMS) selectedIndex = 0;
      
      renderMainMenu(selectedIndex);
    }

    // 2. Select Option on Button Press
    if (isButtonPressed()) {
      return (MenuChoice)selectedIndex;
    }

    // 3. Extra Button acts as a "Back / Exit to Gallery" key
    if (isExtraButtonPressed()) {
      return MENU_GALLERY;
    }

    delay(10);
  }
}

// --- SETTINGS SUBMENU IMPLEMENTATION ---
static void renderSettingsMenu(int selectedIndex, bool isEditing, int speedVal, int timeoutVal) {
  tft.fillScreen(ST77XX_BLACK);

  tft.setFont();
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(45, 15);
  tft.print("-- SETTINGS --");

  String labels[] = {
    "Speed: " + String(speedVal) + "ms",
    "Sleep: " + String(timeoutVal) + "m",
    "Save & Exit"
  };

  int boxWidth = 220;
  int boxHeight = 44;
  int startX = (TFT_WIDTH - boxWidth) / 2;
  int startY = 55;
  int gapY = 54;

  for (int i = 0; i < 3; i++) {
    int itemY = startY + (i * gapY);

    if (i == selectedIndex) {
      if (isEditing) {
        // EDIT MODE: Green border outline with white background
        tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_WHITE);
        tft.drawRect(startX, itemY, boxWidth, boxHeight, ST77XX_GREEN);
        tft.drawRect(startX + 1, itemY + 1, boxWidth - 2, boxHeight - 2, ST77XX_GREEN);
        tft.setTextColor(ST77XX_BLACK);
      } else {
        // FOCUSED: Inverted block
        tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_WHITE);
        tft.setTextColor(ST77XX_BLACK);
      }
    } else {
      tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_BLACK);
      tft.drawRect(startX, itemY, boxWidth, boxHeight, ST77XX_DARKGREY);
      tft.setTextColor(ST77XX_WHITE);
    }

    tft.setTextSize(2);
    tft.setCursor(startX + 15, itemY + 14);
    tft.print(labels[i]);
  }
}

void openSettingsSubmenu() {
  int speed = loadTextSpeed();
  int timeout = loadSleepTimeout();
  
  int selectedIndex = 0;
  bool isEditing = false;
  bool inSettings = true;

  renderSettingsMenu(selectedIndex, isEditing, speed, timeout);

  while (inSettings) {
    int scroll = getEncoderScroll();

    if (scroll != 0) {
      if (!isEditing) {
        selectedIndex += scroll;
        if (selectedIndex < 0) selectedIndex = 2;
        if (selectedIndex > 2) selectedIndex = 0;
      } else {
        // Tweak selected setting value
        if (selectedIndex == 0) {
          speed = constrain(speed + (scroll * 25), 100, 1000);
        } else if (selectedIndex == 1) {
          timeout = constrain(timeout + (scroll * 1), 1, 30);
        }
      }
      renderSettingsMenu(selectedIndex, isEditing, speed, timeout);
    }

    if (isButtonPressed()) {
      if (selectedIndex == 2) {
        // Save & Exit
        inSettings = false;
      } else {
        isEditing = !isEditing; // Toggle parameter edit mode
        renderSettingsMenu(selectedIndex, isEditing, speed, timeout);
      }
    }

    if (isExtraButtonPressed()) {
      inSettings = false; // Cancel out
    }

    delay(10);
  }

  // Save changes to NVS flash
  saveTextSpeed(speed);
  saveSleepTimeout(timeout);
}

bool compareFileItems(const FileItem& a, const FileItem& b) {
  if (a.isDir != b.isDir) return a.isDir; 
  
  int orderA = getBibleBookOrder(a.name);
  int orderB = getBibleBookOrder(b.name);
  if (orderA != 999 || orderB != 999) {
    if (orderA != orderB) return orderA < orderB;
  }
  
  int numA = extractNumber(a.name);
  int numB = extractNumber(b.name);
  if (numA > 0 && numB > 0 && numA != numB) {
    return numA < numB;
  }
  
  return a.name < b.name;
}

String openBibleBrowser(String currentPath, int testamentFilter) {
  std::vector<FileItem> items;
  
  File dir = SD.open(currentPath);
  if (!dir) return "";

  File file = dir.openNextFile();
  while (file) {
    String fileName = String(file.name());
    
    int lastSlash = fileName.lastIndexOf('/');
    if (lastSlash >= 0) fileName = fileName.substring(lastSlash + 1);
    
    if (fileName != "System Volume Information" && 
        fileName != "gallery" && 
        fileName != "media" && 
        !fileName.startsWith(".")) {
      
      // ON-THE-FLY TESTAMENT FILTERING
      int bookOrder = getBibleBookOrder(fileName);
      bool includeFile = true;
      
      if (testamentFilter == 1 && (bookOrder < 1 || bookOrder > 39)) includeFile = false; // Hide NT if OT selected
      if (testamentFilter == 2 && (bookOrder < 40 || bookOrder > 66)) includeFile = false; // Hide OT if NT selected

      if (includeFile) {
        FileItem newItem;
        newItem.name = fileName;
        newItem.isDir = file.isDirectory();
        items.push_back(newItem);
      }
    }
    file.close();
    file = dir.openNextFile();
  }
  dir.close();
  
  std::sort(items.begin(), items.end(), compareFileItems);

  if (items.empty()) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(20, 120);
    tft.print("Empty Folder");
    delay(1000);
    return "";
  }

  int selectedIndex = 0;
  int topIndex = 0;
  const int itemsPerPage = 5;

  while (true) {
    tft.fillScreen(ST77XX_BLACK);
    
    tft.fillRect(0, 0, TFT_WIDTH, 30, ST77XX_DARKGREY);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setFont();
    tft.setCursor(10, 10);
    String title = currentPath;
    if (title == "/") title = "/Bible";
    title.replace('_', ' ');
    tft.print(title);

    for (int i = 0; i < itemsPerPage; i++) {
      int itemIdx = topIndex + i;
      if (itemIdx >= items.size()) break;

      int itemY = 40 + (i * 45);
      String dispName = items[itemIdx].name;
      dispName.replace('_', ' ');

      if (!items[itemIdx].isDir) {
        int dot = dispName.lastIndexOf('.');
        if (dot > 0) dispName = dispName.substring(0, dot);
      }

      if (itemIdx == selectedIndex) {
        tft.fillRect(10, itemY, TFT_WIDTH - 20, 40, ST77XX_WHITE);
        tft.setTextColor(ST77XX_BLACK);
      } else {
        tft.fillRect(10, itemY, TFT_WIDTH - 20, 40, ST77XX_BLACK);
        tft.drawRect(10, itemY, TFT_WIDTH - 20, 40, ST77XX_DARKGREY);
        tft.setTextColor(ST77XX_WHITE);
      }

      tft.setTextSize(2);
      tft.setCursor(20, itemY + 12);
      if (dispName.length() > 16) dispName = dispName.substring(0, 14) + "..";
      tft.print(dispName);
    }

    bool inputReceived = false;
    while (!inputReceived) {
      int scroll = getEncoderScroll();
      if (scroll != 0) {
        selectedIndex += scroll;
        if (selectedIndex < 0) selectedIndex = items.size() - 1;
        if (selectedIndex >= items.size()) selectedIndex = 0;

        if (selectedIndex < topIndex) topIndex = selectedIndex;
        if (selectedIndex >= topIndex + itemsPerPage) topIndex = selectedIndex - itemsPerPage + 1;
        
        inputReceived = true;
      }

      if (isButtonPressed()) {
        if (items[selectedIndex].isDir) {
          String nextPath = currentPath;
          if (!nextPath.endsWith("/")) nextPath += "/";
          nextPath += items[selectedIndex].name;
          
          // Pass 0 to the filter so we don't accidentally hide chapters once inside a book
          String result = openBibleBrowser(nextPath, 0); 
          if (result.length() > 0) return result; 
          inputReceived = true; 
        } else {
          String filePath = currentPath;
          if (!filePath.endsWith("/")) filePath += "/";
          filePath += items[selectedIndex].name;
          return filePath;
        }
      }

      if (isExtraButtonPressed()) return ""; 
      delay(10);
    }
  }
}

String openLanguageMenu() {
  const char* options[] = {"English (ASV)", "Indonesia (TB)"};
  const String folderNames[] = {"ASV", "TB"}; // Maps directly to your SD card folders
  
  int selectedIndex = 0;

  while (true) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setFont();
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(45, 20);
    tft.print("- LANGUAGE -");

    int boxWidth = 220;
    int boxHeight = 50;
    int startX = (TFT_WIDTH - boxWidth) / 2;
    int startY = 65;
    int gapY = 65;

    for (int i = 0; i < 2; i++) {
      int itemY = startY + (i * gapY);

      if (i == selectedIndex) {
        tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_WHITE);
        tft.setTextColor(ST77XX_BLACK);
      } else {
        tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_BLACK);
        tft.drawRect(startX, itemY, boxWidth, boxHeight, ST77XX_DARKGREY);
        tft.setTextColor(ST77XX_WHITE);
      }

      tft.setTextSize(2);
      tft.setCursor(startX + 15, itemY + 18);
      tft.print(options[i]);
    }

    bool inputReceived = false;
    while (!inputReceived) {
      int scroll = getEncoderScroll();
      if (scroll != 0) {
        selectedIndex += scroll;
        if (selectedIndex < 0) selectedIndex = 1;
        if (selectedIndex > 1) selectedIndex = 0;
        inputReceived = true; 
      }

      if (isButtonPressed()) return folderNames[selectedIndex]; 
      if (isExtraButtonPressed()) return ""; // Cancel
      delay(10);
    }
}
}

int openTestamentMenu() {
  const char* options[] = {"1. Old Testament", "2. New Testament"};
  int selectedIndex = 0;

  while (true) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setFont();
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(40, 20);
    tft.print("- TESTAMENT -");

    int boxWidth = 220;
    int boxHeight = 50;
    int startX = (TFT_WIDTH - boxWidth) / 2;
    int startY = 65;
    int gapY = 65;

    for (int i = 0; i < 2; i++) {
      int itemY = startY + (i * gapY);

      if (i == selectedIndex) {
        tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_WHITE);
        tft.setTextColor(ST77XX_BLACK);
      } else {
        tft.fillRect(startX, itemY, boxWidth, boxHeight, ST77XX_BLACK);
        tft.drawRect(startX, itemY, boxWidth, boxHeight, ST77XX_DARKGREY);
        tft.setTextColor(ST77XX_WHITE);
      }

      tft.setTextSize(2);
      tft.setCursor(startX + 15, itemY + 18);
      tft.print(options[i]);
    }

    bool inputReceived = false;
    while (!inputReceived) {
      int scroll = getEncoderScroll();
      if (scroll != 0) {
        selectedIndex += scroll;
        if (selectedIndex < 0) selectedIndex = 1;
        if (selectedIndex > 1) selectedIndex = 0;
        inputReceived = true; 
      }

      if (isButtonPressed()) return selectedIndex + 1; // 1 for OT, 2 for NT
      if (isExtraButtonPressed()) return 0; // Cancel
      delay(10);
    }
  }
}

