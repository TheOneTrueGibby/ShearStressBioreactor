
static constexpr double CHANNEL_HEIGHT = 0.0005; // m
static constexpr double CHANNEL_WIDTH = 0.04; // m
static constexpr double MU = 0.0006922; // Pa * s
static constexpr double RHO = 993; // kg / m^3

double flowRateCalc(double sesnorValue) {
    double fl = (((CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT * sesnorValue) / (6 * MU)) * 6e7)/32;
    return fl;
}