#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0 // 1 = ON, 0 = OFF -> Just for testing, pleas ignore it

// This functions are used for parsing input
char replaceLetter(char letter);

int scanCards(char *cards, int amount, int isFirst);

int loadAndParse(char *myHand, char *opponentHand);

// Evaluation functions
int cardStrategy(const char *myHand, const char *opponentHand);

char evaluateStraight(const char *myHand, const char *opponentHand, int);

char evaluateFullHouse(const char *myHand, const char *opponentHand);

char evaluateXOfKind(const char *myHand, const char *opponentHand, int x);

char evaluateFlush(const char *myHand, const char *opponentHand);

char evaluateTwoPairs(const char *myHand, const char *opponentHand);

char evaluateHighCard(const char *myHand, const char *opponentHand);

// Other functions
int straightAlg(const char *cards, int ignoreColor);

int straightWithA(const char *cards, int ignoreColor);

int xOfKind(const char *cards, int amount, char *specialCards, int numberOfSpecialCards, int size);

int flushAlg(const char *cards, char *usedCards);

int main(void)
{
    char myHand[14] = { 0 };
    char opponentHand[14] = { 0 };

    while (!loadAndParse(myHand, opponentHand)) {
        if (DEBUG) {
            printf("\nMy hand: ");
            for (int i = 0; i < 7; i++) {
                printf("%c%c ", myHand[i * 2], myHand[i * 2 + 1]);
            }

            printf("\nOpponent hand: ");
            for (int i = 0; i < 7; i++) {
                printf("%c%c ", opponentHand[i * 2], opponentHand[i * 2 + 1]);
            }
            printf("\n");
        }

        if (cardStrategy(myHand, opponentHand)) {
            return 0; // Something went wrong. How ? IDK
        }
    }
    return 0; // Invalid card or duplicity in cards or EOF
}

int scanCards(char *cards, int amount, int isFirst)
{
    char card[3] = { 0 };
    for (int i = 0; i < amount; i++) {
        if (scanf(" %c%c%c", &card[0], &card[1], &card[2]) == EOF) {
            if (!isFirst || i > 0) {
                fprintf(stderr, "Invalid amount of cards.\n");
            }
            return 1; // Error - EOF
        }
        if (!strchr("23456789TJQKA", card[0]) || !strchr("hdsc", card[1]) || !isspace(card[2]) ||
                (i == amount - 1 && card[2] != '\n') || card[0] == '\0' || card[1] == '\0') {
            fprintf(stderr, "Invalid card input: %c%c.\n", card[0], card[1]);
            return 1; // Error - invalid card input
        }

        cards[i * 2 + 1] = card[1];            // Card color/type
        cards[i * 2] = replaceLetter(card[0]); // Card number
    }
    return 0; // OK
}

char replaceLetter(char letter)
{
    if (strchr("23456789", letter)) {
        return letter; // Ignore replace function
    }
    switch (letter) {
    case 'T':
        return ':'; // Next char after '9'
    case 'J':
        return ';'; // Next char after ':'
    case 'Q':
        return '<'; // Next char after ';'
    case 'K':
        return '='; // Next char after '<'
    default:
        return '>'; // Highest card (A), next char after '='
    }
}

int loadAndParse(char *myHand, char *opponentHand)
{
    char table[10] = { 0 }; //Cards on table

    if (scanCards(myHand, 2, 1) || scanCards(opponentHand, 2, 0) || scanCards(table, 5, 0)) {
        return 1; // Error - Invalid input
    }

    if ((myHand[1] == myHand[3] && myHand[2] == myHand[0]) ||
            (opponentHand[1] == opponentHand[3] && opponentHand[2] == opponentHand[0])) {
        fprintf(stderr, "Found duplicity on hand.\n");
        return 1; // Error - duplicity on hands
    }

    for (int j = 0; j < 2; j++) { //Check for duplicity with hands
        for (int i = 0; i < 5; i++) {
            if ((myHand[j * 2 + 1] == table[i * 2 + 1] && myHand[j * 2] == table[i * 2]) ||
                    (opponentHand[j * 2 + 1] == table[i * 2 + 1] && opponentHand[j * 2] == table[i * 2])) {
                fprintf(stderr, "Found duplicity with table and hand.\n");
                return 1; // Error - duplicity with table and hands
            }
        }
    }

    for (int j = 0; j < 2; j++) { // Check for duplicity with hands
        for (int i = 0; i < 5; i++) {
            if ((myHand[j * 2 + 1] == table[i * 2 + 1] && myHand[j * 2] == table[i * 2])) {
                fprintf(stderr, "Found duplicity on table.\n");
                return 1; // Error - duplicity on table
            }
        }
    }

    for (int i = 2; i < 7; i++) { // Merge cards from table with hands
        myHand[i * 2] = table[(i - 2) * 2];
        myHand[i * 2 + 1] = table[(i - 2) * 2 + 1];

        opponentHand[i * 2] = table[(i - 2) * 2];
        opponentHand[i * 2 + 1] = table[(i - 2) * 2 + 1];
    }

    char swapElement; // Temporary variable used in bubble sort
    for (int j = 0; j < 7; j++) {
        for (int i = 0; i < 6; i++) {
            if (myHand[i * 2] > myHand[i * 2 + 2]) {
                // Swap for color/type (me)
                swapElement = myHand[i * 2 + 1];
                myHand[i * 2 + 1] = myHand[i * 2 + 3];
                myHand[i * 2 + 3] = swapElement;

                // Swap for card number (me)
                swapElement = myHand[i * 2];
                myHand[i * 2] = myHand[i * 2 + 2];
                myHand[i * 2 + 2] = swapElement;
            }

            if (opponentHand[i * 2] > opponentHand[i * 2 + 2]) {
                //Swap for card color/type (opponent)
                swapElement = opponentHand[i * 2 + 1];
                opponentHand[i * 2 + 1] = opponentHand[i * 2 + 3];
                opponentHand[i * 2 + 3] = swapElement;

                //Swap for card number (opponent)
                swapElement = opponentHand[i * 2];
                opponentHand[i * 2] = opponentHand[i * 2 + 2];
                opponentHand[i * 2 + 2] = swapElement;
            }
        }
    }
    return 0; // OK
}

int straightAlg(const char *cards, int ignoreColor)
{
    int actualCard = 0;
    int counter = 0; // Counter for founded cards
    int maxCard = 0; // Value of the highest founded card

    for (int i = 0; i < 7; i++) {
        counter = 1; // This must be 1 to include actual card
        actualCard = cards[i * 2];
        for (int j = 0; j < 7; j++) {
            if (actualCard + 1 == cards[j * 2] && (cards[i * 2 + 1] == cards[j * 2 + 1] || ignoreColor)) {
                actualCard++; // Move to next card/char
                counter++;
            }
        }

        if (counter == 5 && actualCard > maxCard) {
            maxCard = actualCard; // Swap if found better card
        }
    }
    return maxCard;
}

int straightWithA(const char *cards, int ignoreColor)
{
    int withoutA = straightAlg(cards, ignoreColor); // Evaluate with A as the highest card
    char copyOfCards[14] = { 0 };                   // Array used to evaluate A as the lowest card
    int withA = 0;

    if (cards[12] == '>') { // Highest A == '>'
        for (int i = 0; i < 7; i++) {
            copyOfCards[i * 2 + 1] = cards[i * 2 + 1]; // Color/type
            if (cards[i * 2] == '>') {                 // Any A in array
                copyOfCards[i * 2] = '1';              // Replace the highest card with the lowest
            } else {
                copyOfCards[i * 2] = cards[i * 2];
            }
        }
        withA = straightAlg(copyOfCards, ignoreColor); // Evaluate with A as the lowest card
    }

    if (withoutA > withA) { // Check for bigger value
        return withoutA;
    }
    return withA;
}

char evaluateStraight(const char *myHand, const char *opponentHand, int disableColor)
{
    int straightMe = straightWithA(myHand, disableColor);             // hand, ignore_color = True/False
    int straightOpponent = straightWithA(opponentHand, disableColor); // hand, ignore_color = True/False

    if (straightMe > straightOpponent) {
        return 'W'; // Win for me
    }

    if (straightMe < straightOpponent) {
        return 'L'; // Win for opponent
    }

    if (straightMe == straightOpponent && straightMe) {
        return 'D'; // Equal cards
    }
    return 'N'; // None
}

char evaluateFullHouse(const char *myHand, const char *opponentHand)
{
    char leftOverCardsMe[8] = { 0 };       // Temporary variable for me
    char leftOverCardsOpponent[8] = { 0 }; // Temporary variable for opponent

    int threeMe = xOfKind(myHand, 3, leftOverCardsMe, 4, 7);
    int threeOpponent = xOfKind(opponentHand, 3, leftOverCardsOpponent, 4, 7);

    if (DEBUG) {
        printf("\n**** Leftover Cards - Full house ****\n");
        printf("\nMy cards: ");
        for (int j = 0; j < 4; j++) {
            printf("%c%c ", leftOverCardsMe[j * 2], leftOverCardsMe[j * 2 + 1]);
        }

        printf("\nOpponent's cards: ");
        for (int j = 0; j < 4; j++) {
            printf("%c%c ", leftOverCardsOpponent[j * 2], leftOverCardsOpponent[j * 2 + 1]);
        }
        printf("\n");
    }

    int twoMeLow = xOfKind(leftOverCardsMe, 2, NULL, 0, 4);
    int threeMeLow = xOfKind(leftOverCardsMe, 3, NULL, 0, 4); // Tree cards instead of two !

    if (threeMeLow > twoMeLow) {
        twoMeLow = threeMeLow; // Swap cards
    }

    int twoOpponentLow = xOfKind(leftOverCardsOpponent, 2, NULL, 0, 4);
    int threeOpponentLow = xOfKind(leftOverCardsOpponent, 3, NULL, 0, 4); // Tree cards instead of two !

    if (threeOpponentLow > twoOpponentLow) {
        twoOpponentLow = threeOpponentLow; // Swap cards
    }

    if (DEBUG) {
        printf("\nFull House (low cards) -> My: %d, Opponent: %d\n", twoMeLow, twoOpponentLow);
    }

    if (threeMe > threeOpponent && twoMeLow) {
        return 'W'; // Win for me, higher card
    }

    if (threeMe < threeOpponent && twoOpponentLow) {
        return 'L'; // Win for opponent, higher card
    }

    if (twoMeLow || twoOpponentLow) {
        if (twoMeLow > twoOpponentLow) {
            return 'W'; // Win for me, higher (low) card
        }

        if (twoMeLow < twoOpponentLow) {
            return 'L'; // Win for opponent, higher (low) card
        }
        if (twoMeLow == twoOpponentLow && threeMe == threeOpponent) {
            return 'D'; // Have same special cards (numbers)
        }
    }

    return 'N'; // None
}

int xOfKind(const char *cards, int amount, char *specialCards, int numberOfSpecialCards, int size)
{
    int counter = 0; // Counter for founded cards
    int maxCard = 0; // Value of the highest founded card

    for (int i = 0; i < size; i++) {
        counter = 0;
        for (int j = 0; j < size; j++) {
            if (cards[i * 2] == cards[j * 2]) {
                counter++; // Found matching card
            }
        }
        if (counter == amount && cards[i * 2] >= maxCard) {
            maxCard = cards[i * 2];
        }
    }

    if (maxCard != 0 && specialCards != NULL) {
        counter = 0; // Now used for card position
        for (int i = size - 1; i >= 0; i--) {
            if (cards[i * 2] != maxCard) {
                if (numberOfSpecialCards != 5 && numberOfSpecialCards != 4) { // Every xOfKind
                    specialCards[counter] = cards[i * 2];
                } else {                                                                           // Special case in two pairs / full house
                    specialCards[(numberOfSpecialCards - counter - 1) * 2] = cards[i * 2];         // Card
                    specialCards[(numberOfSpecialCards - counter - 1) * 2 + 1] = cards[i * 2 + 1]; // Color
                }
                counter++;
            }
            if (counter == numberOfSpecialCards) {
                break; // Found enough high cards
            }
        }
    }
    return maxCard;
}

char evaluateXOfKind(const char *myHand, const char *opponentHand, int x)
{
    char specialCardsMe[5 - x];
    int myHighestCard = xOfKind(myHand, x, specialCardsMe, 5 - x, 7);

    char specialCardsOpponent[5 - x];
    int opponentHighestCard = xOfKind(opponentHand, x, specialCardsOpponent, 5 - x, 7);

    if (myHighestCard > opponentHighestCard) {
        return 'W'; // Win for me
    }

    if (myHighestCard < opponentHighestCard) {
        return 'L'; // Win for opponent
    }

    if (opponentHighestCard == myHighestCard && myHighestCard) {
        if (DEBUG) {
            printf("\n**** Special Cards - %d of kind (just numbers) ****\n", x);
            printf("\nMy cards: ");
            for (int j = 0; j < 5 - x; j++) {
                printf("%c ", specialCardsMe[j]);
            }

            printf("\nOpponent's cards: ");
            for (int j = 0; j < 5 - x; j++) {
                printf("%c ", specialCardsOpponent[j]);
            }
            printf("\n");
        }

        for (int j = 0; j < 5 - x; j++) { // Cards are sorted !
            if (specialCardsMe[j] > specialCardsOpponent[j]) {
                return 'W'; // Win for me, better special card
            }

            if (specialCardsMe[j] < specialCardsOpponent[j]) {
                return 'L'; // Win for opponent, better special card
            }
        }
        return 'D'; // Have same special cards (numbers)
    }
    return 'N'; // None
}

int flushAlg(const char *cards, char *usedCards)
{
    int counter = 0;

    for (int j = 6; j >= 0; j--) { // Cards are sorted !
        counter = 0;
        for (int i = 6; i >= 0; i--) {
            if (cards[j * 2 + 1] == cards[i * 2 + 1]) { // Checking for matching color
                usedCards[counter] = cards[i * 2];      // Card number
                counter++;
            }

            if (counter == 5) {
                return 1; // Found enough cards
            }
        }
    }
    return 0; // Not enough same cards
}

char evaluateFlush(const char *myHand, const char *opponentHand)
{
    char usedCardsMe[5] = { 0 };
    int flushMe = flushAlg(myHand, usedCardsMe);

    char usedCardsOpponent[5];
    int flushOpponent = flushAlg(opponentHand, usedCardsOpponent);

    if (flushMe > flushOpponent) {
        return 'W'; // Win for me, flush
    }

    if (flushMe < flushOpponent) {
        return 'L'; // Win for opponent, flush
    }

    if (flushMe && flushOpponent) { // Both players have flush
        if (DEBUG) {
            printf("\n**** Used Cards - Flush (just numbers) ****\n");
            printf("\nMy cards: ");
            for (int j = 0; j < 5; j++) {
                printf("%c ", usedCardsMe[j]);
            }

            printf("\nOpponent's cards: ");
            for (int j = 0; j < 5; j++) {
                printf("%c ", usedCardsOpponent[j]);
            }
            printf("\n");
        }

        for (int j = 0; j < 5; j++) {
            if (usedCardsMe[j] > usedCardsOpponent[j]) {
                return 'W'; // Win for me, higher card
            }

            if (usedCardsMe[j] < usedCardsOpponent[j]) {
                return 'L'; // Win for opponent, higher card
            }
        }
        return 'D'; // Have same cards (numbers)
    }
    return 'N'; // None
}

char evaluateTwoPairs(const char *myHand, const char *opponentHand)
{
    char leftOverCardsMe[10] = { 0 };       // Temporary variable for me
    char leftOverCardsOpponent[10] = { 0 }; // Temporary variable for opponent

    int highTwoMe = xOfKind(myHand, 2, leftOverCardsMe, 5, 7);
    int highTwoOpponent = xOfKind(opponentHand, 2, leftOverCardsOpponent, 5, 7);

    if (DEBUG) {
        printf("\n**** Leftover Cards - Two pairs ****\n");
        printf("\nMy cards: ");
        for (int j = 0; j < 5; j++) {
            printf("%c%c ", leftOverCardsMe[j * 2], leftOverCardsMe[j * 2 + 1]);
        }

        printf("\nOpponent's cards: ");
        for (int j = 0; j < 5; j++) {
            printf("%c%c ", leftOverCardsOpponent[j * 2], leftOverCardsOpponent[j * 2 + 1]);
        }
        printf("\n");
    }

    char specialCardsMe[3] = { 0 };
    int lowTwoMe = xOfKind(leftOverCardsMe, 2, specialCardsMe, 3, 5);

    char specialCardsOpponent[3] = { 0 };
    int lowTwoOpponent = xOfKind(leftOverCardsOpponent, 2, specialCardsOpponent, 3, 5);

    if (DEBUG) {
        printf("\nTwo Pairs (low cards) -> My: %d, Opponent: %d\n", lowTwoMe, lowTwoOpponent);
    }

    if ((highTwoMe && lowTwoMe) || (highTwoOpponent && lowTwoOpponent)) {
        if (highTwoMe > highTwoOpponent && lowTwoMe) {
            return 'W'; // Win for me, higher high card
        }

        if (highTwoMe < highTwoOpponent && lowTwoOpponent) {
            return 'L'; // Win for opponent, higher high card
        }

        if (lowTwoMe > lowTwoOpponent && highTwoMe) {
            return 'W'; // Win for me, higher low card
        }

        if (lowTwoMe < lowTwoOpponent && highTwoOpponent) {
            return 'L'; // Win for opponent, higher low card
        }

        if (highTwoMe == highTwoOpponent && lowTwoMe == lowTwoOpponent) {
            if (DEBUG) {
                printf("\n**** Special Cards - Two pairs (just numbers) ****\n");
                printf("\nMy cards: ");
                for (int j = 0; j < 3; j++) {
                    printf("%c ", specialCardsMe[j]);
                }

                printf("\nOpponent's cards: ");
                for (int j = 0; j < 3; j++) {
                    printf("%c ", specialCardsOpponent[j]);
                }
                printf("\n");
            }

            if (specialCardsMe[0] > specialCardsOpponent[0]) { // 5. card
                return 'W';                                    // Win for me, higher special card
            }

            if (specialCardsMe[0] < specialCardsOpponent[0]) { // 5. card
                return 'L';                                    // Win for opponent, higher special card
            }
            return 'D'; // Have same special cards (numbers)
        }
    }
    return 'N'; // None
}

char evaluateHighCard(const char *myHand, const char *opponentHand)
{
    for (int j = 6; j >= 2; j--) {
        if (myHand[j * 2] > opponentHand[j * 2]) {
            return 'W'; // Win for me, higher card
        }

        if (myHand[j * 2] < opponentHand[j * 2]) {
            return 'L'; // Win for opponent, higher card
        }
    }
    return 'D'; // Have same cards
}

int cardStrategy(const char *myHand, const char *opponentHand)
{
    //Straight Flush
    char outputStraightFlush = evaluateStraight(myHand, opponentHand, 0);
    if (outputStraightFlush != 'N') {
        if (DEBUG) {
            printf("Straight flush: ");
        }
        printf("%c\n", outputStraightFlush);
        return 0; // OK
    }

    // Four of kind
    char outputFourOfKind = evaluateXOfKind(myHand, opponentHand, 4);
    if (outputFourOfKind != 'N') {
        if (DEBUG) {
            printf("Four of kind: ");
        }
        printf("%c\n", outputFourOfKind);
        return 0; // OK
    }

    // Full House
    char outputFullHouse = evaluateFullHouse(myHand, opponentHand);
    if (outputFullHouse != 'N') {
        if (DEBUG) {
            printf("Full House: ");
        }
        printf("%c\n", outputFullHouse);
        return 0; // OK
    }

    // Flush
    char outputFlush = evaluateFlush(myHand, opponentHand);
    if (outputFlush != 'N') {
        if (DEBUG) {
            printf("Flush: ");
        }
        printf("%c\n", outputFlush);
        return 0; // OK
    }

    // Straight
    char outputStraight = evaluateStraight(myHand, opponentHand, 1);
    if (outputStraight != 'N') {
        if (DEBUG) {
            printf("Straight: ");
        }
        printf("%c\n", outputStraight);
        return 0; // OK
    }

    // Three of kind
    char outputThreeOfKind = evaluateXOfKind(myHand, opponentHand, 3);
    if (outputThreeOfKind != 'N') {
        if (DEBUG) {
            printf("Three of kind: ");
        }
        printf("%c\n", outputThreeOfKind);
        return 0; // OK
    }

    // Two pairs
    char outputTwoPair = evaluateTwoPairs(myHand, opponentHand);
    if (outputTwoPair != 'N') {
        if (DEBUG) {
            printf("Two pairs: ");
        }
        printf("%c\n", outputTwoPair);
        return 0; // OK
    }

    // Two of kind (pair)
    char outputTwoOfKind = evaluateXOfKind(myHand, opponentHand, 2);
    if (outputTwoOfKind != 'N') {
        if (DEBUG) {
            printf("Two of kind (pair): ");
        }
        printf("%c\n", outputTwoOfKind);
        return 0; // OK
    }

    // High card
    if (DEBUG) {
        printf("High card: ");
    }
    printf("%c\n", evaluateHighCard(myHand, opponentHand));
    return 0; // OK
}
