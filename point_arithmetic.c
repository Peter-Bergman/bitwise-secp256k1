#include "f_p_arithmetic.c"

typedef struct {
    int *x;
    int *y;
} Point;

bool determineIfPointIsPointAtInfinity(Point *point) {
    int *zero = (int*)calloc(BIT_COUNT, sizeof(int));

    bool isXEqualToZero = cmp(point->x, zero) == 0;
    bool isYEqualToZero = cmp(point->y, zero) == 0;

    free(zero);
    return isXEqualToZero && isYEqualToZero;
}

void setPointToPointAtInfinity(Point *point) {
    int *zero = (int*)calloc(BIT_COUNT, sizeof(int));

    copyBigNumber(point->x, zero);
    copyBigNumber(point->y, zero);

    free(zero);
}

bool determineIfPointsAreInverses(Point *point1, Point *point2) {
    bool areXsEqual = cmp(point1->x, point2->x) == 0;

    int *p = convert_binary_string_to_f_p_element(p_binary_string);
    int *negativePoint2Y = (int*)malloc(sizeof(int) * BIT_COUNT);
    subtractIntegers(negativePoint2Y, p, point2->y);
    bool areYsOpposites = cmp(point1->y, negativePoint2Y) == 0;

    free(p);
    free(negativePoint2Y);

    return areXsEqual && areYsOpposites;
}

bool determineIfPointsAreDistinct(Point *point1, Point *point2) {
    bool areXsUnequal = cmp(point1->x, point2->x) != 0;
    bool areYsUnequal = cmp(point1->y, point2->y) != 0;

    return areXsUnequal || areYsUnequal;
}

void copyPoint(Point *destination, Point *source) {
    copyBigNumber(destination->x, source->x);
    copyBigNumber(destination->y, source->y);
}

void calculateLambdaOfDistinctPoints(int lambda[BIT_COUNT], Point *point1, Point *point2) {
    int *numerator = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *denominator = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *reciprocalOfDenominator = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *p = convert_binary_string_to_f_p_element(p_binary_string);

    // numerator = y2 - y1
    subtractF_pElements(numerator, point2->y, point1->y, p);
    // denominator = x2 - x1
    subtractF_pElements(denominator, point2->x, point1->x, p);
    // reciprocalOfDenominator = 1 / (x2 - x1), where "/" denotes modular multiplicative inverse
    inverseModulo(reciprocalOfDenominator, denominator, p);

    multiplyF_pElements(lambda, numerator, reciprocalOfDenominator, p);

    free(numerator);
    free(denominator);
    free(reciprocalOfDenominator);
    free(p);
}

void calculateX_3ForPointAddition(
    int x_3[BIT_COUNT], int lambda[BIT_COUNT], int x_1[BIT_COUNT], int x_2[BIT_COUNT]) {

    int *lambdaSquared = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *lambdaSquaredMinusX_1 = (int*)malloc(sizeof(int) * BIT_COUNT);

    int *p = convert_binary_string_to_f_p_element(p_binary_string);
    multiplyF_pElements(lambdaSquared, lambda, lambda, p);
    subtractF_pElements(lambdaSquaredMinusX_1, lambdaSquared, x_1, p);
    subtractF_pElements(x_3, lambdaSquaredMinusX_1, x_2, p);

    free(lambdaSquared);
    free(lambdaSquaredMinusX_1);
    free(p);
}

void calculateY_3ForPointAddition(
    int y_3[BIT_COUNT], int lambda[BIT_COUNT],
    int x_1[BIT_COUNT], int x_3[BIT_COUNT], int y_1[BIT_COUNT]) {

    int *tmp1 = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *tmp2 = (int*)malloc(sizeof(int) * BIT_COUNT);

    int *p = convert_binary_string_to_f_p_element(p_binary_string);
    // x_1 - x_3
    subtractF_pElements(tmp1, x_1, x_3, p);
    // lambda * (x_1 - x_3)
    multiplyF_pElements(tmp2, tmp1, lambda, p);
    // y_3 = lambda * (x_1 - x_3)
    subtractF_pElements(y_3, tmp2, y_1, p);

    free(tmp1);
    free(tmp2);
    free(p);
}

void calculateLambdaForPointDoubling(
    int lambda[BIT_COUNT], Point *point1
    // int a[BIT_COUNT] // a is 0 for secp256k1
    ) {

    // x^2
    int *xSquared = (int*)malloc(sizeof(int) * BIT_COUNT);
    // 3x^2 + a
    int *numerator = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *denominator = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *reciprocalOfDenominator = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *three = (int*)calloc(BIT_COUNT, sizeof(int)); // = 0
    three[0] = 1;// + 1 = 1
    three[1] = 1;// + 2 = 3
    int *two = (int*)calloc(BIT_COUNT, sizeof(int)); // = 0
    two[1] = 1; // + 2 = 2

    int *p = convert_binary_string_to_f_p_element(p_binary_string);
    multiplyF_pElements(xSquared, point1->x, point1->x, p);
    multiplyF_pElements(numerator, three, xSquared, p);
    multiplyF_pElements(denominator, two, point1->y, p);
    inverseModulo(reciprocalOfDenominator, denominator, p);
    multiplyF_pElements(lambda, numerator, reciprocalOfDenominator, p);

    free(xSquared);
    free(numerator);
    free(denominator);
    free(reciprocalOfDenominator);
    free(three);
    free(two);
    free(p);
}

void add_points(Point *sum, Point *addend1, Point *addend2) {
    bool isAddend1PointAtInfinity = determineIfPointIsPointAtInfinity(addend1);
    bool isAddend2PointAtInfinity = determineIfPointIsPointAtInfinity(addend2);

    bool areAddendPointsInverses = determineIfPointsAreInverses(addend1, addend2);

    bool arePointsDistinct = determineIfPointsAreDistinct(addend1, addend2);

    // handle point@infinity
    if(isAddend1PointAtInfinity) {
        copyPoint(sum, addend2);
    }
    else if(isAddend2PointAtInfinity) {
        copyPoint(sum, addend1);
    }
    // handle (x,y) + (x,-y)
    else if(areAddendPointsInverses) {
        setPointToPointAtInfinity(sum);
    }
    // handle (x1,y1) + (x2,y2)
    else if(arePointsDistinct) {
        int *lambda = (int*)malloc(sizeof(int) * BIT_COUNT);
        int *x_3 = (int*)malloc(sizeof(int) * BIT_COUNT);
        int *y_3 = (int*)malloc(sizeof(int) * BIT_COUNT);

        calculateLambdaOfDistinctPoints(lambda, addend1, addend2);
        calculateX_3ForPointAddition(x_3, lambda, addend1->x, addend2->x);
        calculateY_3ForPointAddition(y_3, lambda, addend1->x, x_3, addend1->y);

        copyBigNumber(sum->x, x_3);
        copyBigNumber(sum->y, y_3);

        free(lambda);
        free(x_3);
        free(y_3);
    }
    // handle (x,y) + (x,y), point doubling
    else {
        int *lambda = (int*)malloc(sizeof(int) * BIT_COUNT);
        int *x_3 = (int*)malloc(sizeof(int) * BIT_COUNT);
        int *y_3 = (int*)malloc(sizeof(int) * BIT_COUNT);

        calculateLambdaForPointDoubling(lambda, addend1);
        calculateX_3ForPointAddition(x_3, lambda, addend1->x, addend2->x);
        calculateY_3ForPointAddition(y_3, lambda, addend1->x, x_3, addend1->y);

        copyBigNumber(sum->x, x_3);
        copyBigNumber(sum->y, y_3);

        free(lambda);
        free(x_3);
        free(y_3);
    }
}

char *gxBinStr = "111100110111110011001100111111011111001110111001011101110101100010101011010000001100010100101011100111010000111000010110000011100000010100110111111110011011011001011011100111000101000110110010101100111110010100000010101101100010110111110000001011110011000";
char *gyBinStr = "100100000111010110110100111011100100110101000111100010001100101010111011010010011111011111111000000111000010001000010001010100011111101000101111011010001001000101001101000010101010100000110011001110001000111110100001000111111111011000100001101010010111000";

Point *buildPoint() {
    int *x = (int*)malloc(sizeof(int) * BIT_COUNT);
    int *y = (int*)malloc(sizeof(int) * BIT_COUNT);

    Point *newPoint = (Point*)malloc(sizeof(Point));
    newPoint->x = x;
    newPoint->y = y;

    return newPoint;
}

Point *multiply_point_by_scalar_naively(int scalar) {
    Point basePoint = {
        convert_binary_string_to_f_p_element(gxBinStr),
        convert_binary_string_to_f_p_element(gyBinStr)
    };

    Point *basePointMultiplied = buildPoint();
    setPointToPointAtInfinity(basePointMultiplied);

    for(int i = 0; i < scalar; i++) {
        add_points(basePointMultiplied, basePointMultiplied, &basePoint);
    }

    return basePointMultiplied;
}

int main() {

    Point *gCubed = multiply_point_by_scalar_naively(3);

    printF_p("gCubed->x", gCubed->x);
    printF_p("gCubed->y", gCubed->y);
}

