import os
import re

# Open the specific Indonesian TB Bible file you downloaded
filename = "indo_tb (1).txt"

print(f"Opening {filename}...")

try:
    with open(filename, "r", encoding="utf-8") as file:
        lines = file.readlines()
except FileNotFoundError:
    print(f"Error: Could not find '{filename}'. Make sure it is in the same folder as this script.")
    exit()

current_book = ""
current_chapter = ""
output_file = None

# FIXED REGEX: 
# ^(.+?) grabs ANY book name (including "1 Raja-raja")
# \s(\d+):(\d+)\s+(.*) safely grabs the Chapter, Verse, and the actual Text.
pattern = re.compile(r"^(.+?)\s(\d+):(\d+)\s+(.*)")

for line in lines:
    # Skip empty lines immediately
    if not line.strip():
        continue

    match = pattern.match(line.strip())
    
    if match:
        bookName = match.group(1).strip()
        chapterNum = match.group(2)
        verseNum = match.group(3)
        verseText = match.group(4)
        
        book_changed = False
        
        # Did we switch to a new book?
        if bookName != current_book:
            current_book = bookName
            book_changed = True
            
            # Format folder name safely for FAT32 SD Cards (e.g., "1_Raja_raja")
            folder_name = current_book.replace(' ', '_')
            os.makedirs(folder_name, exist_ok=True)
            print(f"Processing: {current_book}...")
            
        # FIXED LOGIC: Create a new file if the chapter OR the book changes!
        if chapterNum != current_chapter or book_changed:
            if output_file:
                output_file.close()
                
            current_chapter = chapterNum
            
            # Create the new chapter file inside the book's folder (e.g., /Kejadian/1.txt)
            file_path = os.path.join(folder_name, f"{current_chapter}.txt")
            output_file = open(file_path, "w", encoding="utf-8")
            
        # Write the formatted verse to the file
        output_file.write(f"{verseNum}. {verseText}\n")

# Close the very last file when the loop finishes
if output_file:
    output_file.close()

print("\nSuccess! The Bible has been sliced and is ready for your ESP32 SD Card.")