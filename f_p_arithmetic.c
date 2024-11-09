#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIT_COUNT 512

int cmp(int comparand1[BIT_COUNT], int comparand2[BIT_COUNT]) {
    for(int index = BIT_COUNT - 1; index >= 0; index--) {
        if(comparand1[index] == comparand2[index]) {
            continue;
        }
        else if(comparand1[index] == 1) {
            // -1 indicates that comparand1 is greater
            return -1;
        }
        else {
            // 1 indicates that comparand2 is greater
            return 1;
        }
    }
    // 0 indicates equality
    return 0;
}

void printF_p(char *label, int f_pElement[BIT_COUNT]) {
    printf("%s:\n", label);
    for(int i = BIT_COUNT - 1; i >= 0; i--) {
        printf("%d", f_pElement[i]);
    }
    printf("\n");
}

char *p_string = "115792089237316195423570985008687907853269984665640564039457584007908834671663";
char *p_binary_string =   "1111111111111111111111111111111111111111111111111111111111111111111111"
                        "111111111111111111111111111111111111111111111111111111111111111111111111"
                        "111111111111111111111111111111111111111111111111111111111111111111111111"
                        "111111111011111111111111111111110000101111";

int *convert_binary_string_to_f_p_element(char *binary_string) {
    int *f_p_element = (int*)calloc(BIT_COUNT, sizeof(int));
    int stringLength = strlen(binary_string);
    // TODO: ensure stringLength is < BIT_COUNT
    for(int i = 0; i < stringLength; i++) {
        if(binary_string[(stringLength - 1) - i] == '1') {
            f_p_element[i] = 1;
        }
        else if(binary_string[i] == '0') {

        }
        // TODO: add else block for parsing error
    }
    return f_p_element;
}

char *convert_f_p_element_to_binary_string(int f_p_element[BIT_COUNT]) {
    char *binary_string = (char*)malloc(sizeof(char) * BIT_COUNT);
    for(int i = 0; i < BIT_COUNT; i++) {
        if(f_p_element[i] == 1) {
            binary_string[(BIT_COUNT - 1) - i] = '1';
        }
        else if(f_p_element[i] == 0) {
            binary_string[(BIT_COUNT - 1) - i] = '0';
        }
    }
    return binary_string;
}

void addIntegers(int sum[BIT_COUNT], int addend1[BIT_COUNT], int addend2[BIT_COUNT]) {
    int carryBit = 0;

    for(int i = 0; i < BIT_COUNT; i++) {
        // addend1 XOR addend2
        int partialSumBit = addend1[i] ^ addend2[i];
        // addend1 AND addend2
        int andAddends = addend1[i] & addend2[i];
        int partialSumBitAndCarryInBit = partialSumBit & carryBit;

        sum[i] = partialSumBit ^ carryBit;
        carryBit = andAddends | partialSumBitAndCarryInBit;
    }
}

int *getOnesComplement(int operand[BIT_COUNT]) {
    int *onesComplement = (int*)malloc(sizeof(int) * BIT_COUNT);

    for(int i = 0; i < BIT_COUNT; i++) {
        if(operand[i] == 0) {
            onesComplement[i] = 1;
        }
        else {
            onesComplement[i] = 0;
        }
    }
    return onesComplement;
}

int *getTwosComplement(int operand[BIT_COUNT]) {
    int *onesComplement = getOnesComplement(operand);

    // set one to represent the integer 1
    int *one = (int*)calloc(BIT_COUNT, sizeof(int));
    one[0] = 1;

    int *twosComplement = (int*)malloc(sizeof(int) * BIT_COUNT);
    addIntegers(twosComplement, onesComplement, one);

    free(onesComplement);
    return twosComplement;
}

void subtractIntegers(int difference[BIT_COUNT], int minuend[BIT_COUNT], int subtrahend[BIT_COUNT]) {
    int *twosComplementOfSubtrahend = getTwosComplement(subtrahend);
    addIntegers(difference, minuend, twosComplementOfSubtrahend);

    free(twosComplementOfSubtrahend);
}

void copyBigNumber(int destination[BIT_COUNT], int source[BIT_COUNT]) {
    for(int i = 0; i < BIT_COUNT; i++) {
        destination[i] = source[i];
    }
}

// NOTE: multiplyIntegers discards all overflow bits
int *multiplyIntegers(int factor1[BIT_COUNT], int factor2[BIT_COUNT]) {
    int *product = (int*)calloc(BIT_COUNT, sizeof(int));

    for(int i = 0; i < BIT_COUNT; i++) {
        if (factor2[i] == 1) {
            int *addend = (int*)calloc(BIT_COUNT, sizeof(int));
            // XXX: I am especially worried about the comparison on the line below being one-off
            for(int j = 0; i + j < BIT_COUNT; j++) {
                addend[i + j] = factor2[i] && factor1[j];
            }

            addIntegers(product, product, addend);

            free(addend);
        }
    }
    return product;
}

int *divideIntegers(int dividend[BIT_COUNT], int divisor[BIT_COUNT]) {
    int *quotient = (int*)calloc(BIT_COUNT, sizeof(int));
    int *remainder = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(remainder, dividend);

    // set one to represent the integer 1
    int *one = (int*)calloc(BIT_COUNT, sizeof(int));
    one[0] = 1;

    while(cmp(remainder, divisor) <= 0) {
        subtractIntegers(remainder, remainder, divisor);

        addIntegers(quotient, quotient, one);
    }
    
    free(remainder);
    free(one);

    return quotient;
}

int *getExponentOfTwo(int exponent) {
    int *exponentiated_2 = (int*)calloc(BIT_COUNT, sizeof(int));
    exponentiated_2[exponent] = 1;
    return exponentiated_2;
}

void divMod(
    int quotient[BIT_COUNT], int remainder[BIT_COUNT],
    int dividend[BIT_COUNT], int divisor[BIT_COUNT]
    ) {
    // initialization
    int *running_quotient = (int*)calloc(BIT_COUNT, sizeof(int));
    int *running_remainder = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(running_remainder, dividend);

    for(int index = 255; index >= 0; index--) {
        int *exponentiated_2 = getExponentOfTwo(index);
        int *scaled_divisor = multiplyIntegers(divisor, exponentiated_2);

        // check if scaled_divisor fits into running_remainder
        if(cmp(scaled_divisor, running_remainder) >= 0) {
            // if so, account for scaled_divisor and move on
            addIntegers(running_quotient, running_quotient, exponentiated_2);
            subtractIntegers(running_remainder, running_remainder, scaled_divisor);
        }
        free(scaled_divisor);
        free(exponentiated_2);
    }

    copyBigNumber(quotient, running_quotient);
    copyBigNumber(remainder, running_remainder);

    free(running_quotient);
    free(running_remainder);
}

bool inverseModulo(int result[BIT_COUNT], int base[BIT_COUNT], int modulus[BIT_COUNT]) {
    int *a = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(a, base);

    int *b = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(b, modulus);

    // initialize zero and one
    int *zero = (int*)calloc(BIT_COUNT, sizeof(int));
    int *one = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(one, zero);
    one[0] = 1;

    // initialize s_a to 1
    int *s_a = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(s_a, one);

    // initialize s_b to 0
    int *s_b = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(s_b, zero);

    int *q = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *r = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *s_r = (int*)malloc(sizeof(int) * BIT_COUNT);

    bool isResultPositive = true;

    while(1==1) {
        // set q and r
        divMod(q, r, b, a);
        // set s_r
        isResultPositive = !isResultPositive; // to account for not negating q
        int *tmp1 = multiplyIntegers(s_a, q);
        addIntegers(s_r, s_b, tmp1);
        free(tmp1);
        
        // if r == 1
        if(cmp(r, one) == 0) {
            // TODO: add mem cleanup

            if (isResultPositive) {
                copyBigNumber(result, s_r);
            }
            else {
                subtractIntegers(result, modulus, s_r);
            }
            // indicate success
            return true;
        }
        // else if r <= 0
        else if(cmp(r, zero) >= 0) {
            // indicate an error
            return false;
        }
        else {
            copyBigNumber(b, a);
            copyBigNumber(a, r);
            copyBigNumber(s_b, s_a);
            copyBigNumber(s_a, s_r);
        }
    }
}

void addF_pElements(
    int sum[BIT_COUNT], int addend1[BIT_COUNT], int addend2[BIT_COUNT], int p[BIT_COUNT]) {

    int *tmp1 = (int*)malloc(sizeof(int) * BIT_COUNT);

    addIntegers(sum, addend1, addend2);
    divMod(tmp1, sum, sum, p);

    free(tmp1);
}

void subtractF_pElements(
    int difference[BIT_COUNT], int minuend[BIT_COUNT], int subtrahend[BIT_COUNT], int p[BIT_COUNT]) {

    // if the minuend is less than the subtrahend (subtracting a bigger # from a smaller #)
    if(cmp(minuend, subtrahend) > 0) {
        int *negativeSubtrahend = (int*)malloc(sizeof(int) * BIT_COUNT);
        subtractIntegers(negativeSubtrahend, p, subtrahend);
        addIntegers(difference, minuend, negativeSubtrahend);

        free(negativeSubtrahend);
    }
    else {
        subtractIntegers(difference, minuend, subtrahend);
    }
}

void multiplyF_pElements(
    int product[BIT_COUNT], int factor1[BIT_COUNT], int factor2[BIT_COUNT], int p[BIT_COUNT]) {

    int *tmp1 = multiplyIntegers(factor1, factor2);
    divMod(tmp1, product, tmp1, p);

    free(tmp1);
}

void testConversions(char *binStr) {
    int *num = convert_binary_string_to_f_p_element(binStr);
    char *binStrConverted = convert_f_p_element_to_binary_string(num);

    printf("TESTING CONVERSIONS\n");
    printf("binStr:\n%s\n", binStr);
    printf("binStrConverted:\n%s\n", binStrConverted);
    printf("Passed? (should equal 0):%d\n\n\n", strcmp(binStr, binStrConverted));

    free(num);
    free(binStrConverted);
}

void testCmp(char *comparand1, char *comparand2, int expectedResult) {
    int *c1 = convert_binary_string_to_f_p_element(comparand1);
    int *c2 = convert_binary_string_to_f_p_element(comparand2);

    int cmpResult = cmp(c1, c2);

    printf("TESTING COMPARISON\n");
    printf("comparand1:\n%s\n", comparand1);
    printf("comparand2:\n%s\n", comparand2);
    printf("Comparison result (expecting %d): %d\n\n\n", expectedResult, cmpResult);

    free(c1);
    free(c2);
}

//void 

void testF_pMath() {
    // conversions test
    int *p = convert_binary_string_to_f_p_element(p_binary_string);
    char *p_string_converted = convert_f_p_element_to_binary_string(p);
    int comp_result = strcmp(p_binary_string, p_string_converted);
    printf("p_binary_string:\n%s\n", p_binary_string);
    printf("p_string_converted:\n%s\n", p_string_converted);
    printf("comp_result: %d\n\n\n", comp_result);

    testConversions(p_binary_string);

    // 54
    char *fiftyFourBinaryString = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110110";
    int *fiftyFour = convert_binary_string_to_f_p_element(fiftyFourBinaryString);
    // 6
    char *sixBinaryString = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110";
    int *six = convert_binary_string_to_f_p_element(sixBinaryString);

    // equality case
    testCmp(p_binary_string, p_binary_string, 0);
    // 1st arg greater
    testCmp(fiftyFourBinaryString, sixBinaryString, -1);
    // 2nd arg greater
    testCmp(sixBinaryString, fiftyFourBinaryString, 1);

    // 54 - 6
    int *fortyEight = (int*)malloc(sizeof(int) * BIT_COUNT);
    subtractIntegers(fortyEight, fiftyFour, six);
    char *fortyEightBinaryString = convert_f_p_element_to_binary_string(fortyEight);
    printf("fortyEightBinaryString:\n%s\n\n\n", fortyEightBinaryString);

    // 6 copied
    int *sixCopied = (int*)malloc(sizeof(int) * BIT_COUNT);
    copyBigNumber(sixCopied, six);
    char *sixCopiedBinStr = convert_f_p_element_to_binary_string(sixCopied);
    printf("sixCopiedBinStr:\n%s\n\n\n", sixCopiedBinStr);

    // 54 divided by 6
    int *nine = divideIntegers(fiftyFour, six);
    char *nineBinaryString = convert_f_p_element_to_binary_string(nine);
    printf("nineBinaryString:\n%s\n", nineBinaryString);

    // 54 divMod by 6
    int *zero = (int*)malloc(sizeof(int) * BIT_COUNT);
    divMod(nine, zero, fiftyFour, six);
    printF_p("54 divMod 6, div", nine);
    printF_p("54 divMod 6, mod", zero);

}

