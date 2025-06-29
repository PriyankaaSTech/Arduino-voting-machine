#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 2, 8};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// -------- VOTING SETUP --------
const int buttonPins[4] = {3, 4, 5, 6};
const int resultButton = 7;
const int ledPins[4] = {9, 10, 11, 12};
const int buzzerPin = 13;

int votes[4] = {0, 0, 0, 0};

// -------- PASSWORD STRUCTURE --------
struct PasswordEntry {
  const char* password;
  bool used;
};

PasswordEntry validPasswords[70] = {
  {"1234", false}, {"4321", false}, {"1111", false}, {"0000", false}, {"2468", false},
  {"1357", false}, {"9999", false}, {"2580", false}, {"9876", false}, {"1010", false},
  {"1001", false}, {"1002", false}, {"1003", false}, {"1004", false}, {"1005", false},
  {"1006", false}, {"1007", false}, {"1008", false}, {"1009", false}, {"1011", false},
  {"1012", false}, {"1013", false}, {"1014", false}, {"1015", false}, {"1016", false},
  {"1017", false}, {"1018", false}, {"1019", false}, {"1020", false}, {"1021", false},
  {"1022", false}, {"1023", false}, {"1024", false}, {"1025", false}, {"1026", false},
  {"1027", false}, {"1028", false}, {"1029", false}, {"1030", false}, {"2001", false},
  {"2002", false}, {"2003", false}, {"2004", false}, {"2005", false}, {"2006", false},
  {"2007", false}, {"2008", false}, {"2009", false}, {"2010", false}, {"2011", false},
  {"2012", false}, {"2013", false}, {"2014", false}, {"2015", false}, {"2016", false},
  {"2017", false}, {"2018", false}, {"2019", false}, {"2020", false}, {"2021", false},
  {"2022", false}, {"2023", false}, {"2024", false}, {"2025", false}, {"2026", false},
  {"2027", false}, {"2028", false}, {"2029", false}, {"2030", false}, {"9990", false}
};

String inputPassword = "";
bool accessGranted = false;
bool voteCasted = false;
int currentUserIndex = 0;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(resultButton, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Enter password to start voting:");
}

void loop() {
  // -------- PASSWORD ENTRY --------
  if (!accessGranted) {
    char key = keypad.getKey();
    if (key) {
      Serial.print("Key pressed: ");
      Serial.println(key);

      if (key == '#') {
        if (currentUserIndex < 70) {
          const char* expectedPassword = validPasswords[currentUserIndex].password;

          if (inputPassword.equals(expectedPassword)) {
            if (!validPasswords[currentUserIndex].used) {
              validPasswords[currentUserIndex].used = true;
              Serial.print("Access Granted for Person #");
              Serial.print(currentUserIndex + 1);
              Serial.println(". You may now vote.");
              playSuccessTone();
              accessGranted = true;
              voteCasted = false;
            } else {
              Serial.println("This password has already been used.");
              playFailureTone();
            }
          } else {
            Serial.print("Incorrect password for Person #");
            Serial.println(currentUserIndex + 1);
            playFailureTone();
          }
        } else {
          Serial.println("No more users allowed.");
          playFailureTone();
        }

        inputPassword = ""; // Reset input
      }
      else if (key == '*') {
        inputPassword = "";
        Serial.println("Input cleared.");
      }
      else {
        inputPassword += key;
      }
    }
  }

  // -------- VOTING PROCESS (One vote per access) --------
  if (accessGranted && !voteCasted) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == HIGH) {
        votes[i]++;
        Serial.print("Vote for Candidate ");
        Serial.print(i + 1);
        Serial.println(" counted!");
        delay(300); // Debounce
        voteCasted = true;
        accessGranted = false;
        currentUserIndex++;  // Move to next user
        Serial.println("Your vote is registered. Next person, please enter your password.");
        break;
      }
    }
  }

  // -------- RESULT CHECK (Always allowed) --------
  if (digitalRead(resultButton) == HIGH) {
    delay(500); // Debounce
    declareWinner();
  }
}

// -------- DECLARE WINNER --------
void declareWinner() {
  int maxVotes = 0;
  int winnerIndex = -1;

  for (int i = 0; i < 4; i++) {
    if (votes[i] > maxVotes) {
      maxVotes = votes[i];
      winnerIndex = i;
    }
  }

  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW); // Turn off all LEDs
  }

  if (maxVotes == 0) {
    Serial.println("No votes were cast. No winner.");
  } else {
    Serial.print("Voting Ended! Winner is Candidate ");
    Serial.print(winnerIndex + 1);
    Serial.print(" with ");
    Serial.print(maxVotes);
    Serial.println(" votes!");
    digitalWrite(ledPins[winnerIndex], HIGH); // Show winner
  }

  Serial.println("System will restart when RESET button is pressed.");
}

// -------- BUZZER FUNCTIONS --------
void playSuccessTone() {
  tone(buzzerPin, 1500, 300);
  delay(350);
  tone(buzzerPin, 2000, 300);
  delay(350);
  noTone(buzzerPin);
}

void playFailureTone() {
  tone(buzzerPin, 2000, 500);
  delay(1200);
  noTone(buzzerPin);
}
