// PROJECT  :ACES RPN Calculator
// PURPOSE  :The purpose of this project is to develop an working RPN calculator
// COURSE   :ICS3U-E
// AUTHOR   :B. MI
// DATE     :2025 04 18
// MCU      :328P (Standalone)
// STATUS   :Working
// REFERENCE: http://darcy.rsgc.on.ca/ACES/TEI3M/Tasks.html#RPN

// Include necessary libraries
#include "Stack.h"        // Custom stack implementation for floating-point numbers
#include <LiquidCrystal.h> // Library for LCD display

// Constants
#define DURATION 300  // Debounce delay for key presses in milliseconds
#define DEBUG    1     // Flag to enable/disable serial debug output

// Initialize LCD display with pin connections
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
Stack stack;        // Create stack instance for storing numbers
int sz = 0;         // Variable to track stack size
char cur[16] = "";  // Store current input

// Analog threshold values for keypad input 
const uint16_t th[] = {
  55,  58,  62,  66,  75,  81,  88,  97,
  116, 132, 152, 179, 255, 341, 512, 1024
};

// Key characters corresponding to threshold values (4x4 matrix layout)
const char keys[] = {
  '+','E','.','0','-','3','2','1',
  '*','6','5','4','/','9','8','7'
};

// Function to display input prompt on LCD
void showPrompt() {
  lcd.clear();          // Clear display
  lcd.print("Enter:");  // Show input prompt on first line
  lcd.setCursor(0,1);   // Move to second line
  lcd.print(cur);       // Display current input buffer
}

// Function to display temporary message on LCD
void showMsg(const char* prefix, const char* message, int duration=800) {
  lcd.clear();
  lcd.print(prefix);    // Show message prefix (e.g., "Error: ")
  lcd.print(message);   // Display main message content
  delay(duration);      // Keep message visible
  showPrompt();         // Return to normal input display
}

// Function to read pressed key from analog keypad
char getKey() {
  uint16_t value;
  do { 
    value = analogRead(A5); // Read analog value from keypad
  } while (value == 0);     // Wait until a key is pressed
  
  delay(DURATION); // Simple debounce delay
  
  if (DEBUG) {     // Debug output if enabled
    Serial.print(value);
    Serial.print('\t');
  }
  
  // Find which key was pressed based on threshold values
  for (uint8_t i = 0; i < 16; i++) {
    if (value <= th[i]) return keys[i];
  }
  return 0; // Should never reach here (fallback)
}

// Initial setup function
void setup() {
  lcd.begin(16, 2);    // Initialize 16x2 LCD
  Serial.begin(9600);  // Start serial communication
  while (!Serial);     // Wait for serial port to connect (for debugging)
  showPrompt();        // Display initial prompt
}

// Main program loop
void loop() {
  char key = getKey(); // Get pressed key
  Serial.println(key); // Debug output

  // Handle numeric input (digits 0-9 and decimal point)
  if (key == '.' || (key >= '0' && key <= '9')) {
    if (strlen(cur) < 15) { // Prevent buffer overflow
      // Check for duplicate decimal points
      if (key == '.' && strchr(cur, '.')) {
        return showMsg("Error: Multiple .", "");
      }
      strncat(cur, &key, 1);  // Append character to input buffer
      lcd.setCursor(0,1);     // Update LCD display
      lcd.print(cur);
    }

  // Handle Enter key or operators
  } else if (key == 'E' || strchr("+-*/", key)) {
    // Push current number to stack if input exists
    if (*cur) {
      float num = atof(cur);      // Convert string to float
      stack.push(num);            // Push to stack
      sz++;                       // Increment stack size counter
      char buffer[16];
      dtostrf(num, 0, 4, buffer); // Convert float to string
      showMsg("Pushed: ", buffer, 1000); // Display confirmation
      cur[0] = '\0';              // Clear input buffer
    }

    // Process operator (if not Enter key)
    if (key != 'E') {
      // Verify stack has enough operands
      if (sz < 2) {
        return showMsg("Error: Stack <2", "");
      }

      // Pop top two values from stack (b is top element)
      float b = stack.pop();
      float a = stack.pop();
      sz -= 2;

      // Handle division by zero special case
      if (key == '/' && b == 0) {
        // Restore original values to stack
        stack.push(a);
        stack.push(b);
        sz += 2;
        return showMsg("Error: Div 0", "");
      }

      // Perform calculation based on symbol
      float result;
      switch(key) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': result = a / b; break;
      }

      // Push result back to stack
      stack.push(result);
      sz++;
      char resultBuffer[16];
      dtostrf(result, 0, 4, resultBuffer); // Convert result to string
      showMsg("Result: ", resultBuffer, 1000); // Display result
    }
  }
}
