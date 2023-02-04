#define NOISE_DATA_END 0xFF

// Individual noise samples, arranged into groups of 3 bytes, with NOISE_DATA_END
// terminating the array when in the first slot.
static const u8 sDrumNoiseDataDrum00[] =
{
    0x20, 0x11, 0x00,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare1[] =
{
    0x20, 0xC1, 0x33,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare2[] =
{
    0x20, 0xB1, 0x33,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare3[] =
{
    0x20, 0xA1, 0x33,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare4[] =
{
    0x20, 0x81, 0x33,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum05[] =
{
    0x27, 0x84, 0x37, 
    0x26, 0x84, 0x36, 
    0x25, 0x83, 0x35, 
    0x24, 0x83, 0x34, 
    0x23, 0x82, 0x33, 
    0x22, 0x81, 0x32,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataTriangle1[] =
{
    0x20, 0x51, 0x2A,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataTriangle2[] =
{
    0x21, 0x41, 0x2B, 
    0x20, 0x61, 0x2A,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataHiHat1[] =
{
    0x20, 0x81, 0x10,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare5[] =
{
    0x20, 0x82, 0x23,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare6[] =
{
    0x20, 0x82, 0x25,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare7[] =
{
    0x20, 0x82, 0x26,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataHiHat2[] =
{
    0x20, 0xA1, 0x10,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataHiHat3[] =
{
    0x20, 0xA2, 0x11,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare8[] =
{
    0x20, 0xA2, 0x50,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataTriangle3[] =
{
    0x20, 0xA1, 0x18, 
    0x20, 0x31, 0x33,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataTriangle4[] =
{
    0x22, 0x91, 0x28, 
    0x20, 0x71, 0x18,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare9[] =
{
    0x20, 0x91, 0x22,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare10[] =
{
    0x20, 0x71, 0x22,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare11[] =
{
    0x20, 0x61, 0x22,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum20[] =
{
    0x20, 0x11, 0x11,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum21[] =
{
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare12[] =
{
    0x20, 0x91, 0x33,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare13[] =
{
    0x20, 0x51, 0x32,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataSnare14[] =
{
    0x20, 0x81, 0x31,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataKick1[] =
{
    0x20, 0x88, 0x6B, 
    0x20, 0x71, 0x00,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataTriangle5[] =
{
    0x30, 0x91, 0x18,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum27[] =
{
    0x27, 0x92, 0x10,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum28[] =
{
    0x33, 0x91, 0x00, 
    0x33, 0x11, 0x00,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum29[] =
{
    0x33, 0x91, 0x11, 
    0x33, 0x11, 0x00,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataCrash1[] =
{
    0x33, 0x88, 0x15, 
    0x20, 0x65, 0x12,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum31[] =
{
    0x33, 0x51, 0x21, 
    0x33, 0x11, 0x11,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum32[] =
{
    0x33, 0x51, 0x50, 
    0x33, 0x11, 0x11,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum33[] =
{
    0x20, 0xA1, 0x31,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataCrash2[] =
{
    0x20, 0x84, 0x12,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum35[] =
{
    0x33, 0x81, 0x00, 
    0x33, 0x11, 0x00,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataDrum36[] =
{
    0x33, 0x81, 0x21, 
    0x33, 0x11, 0x11,
    NOISE_DATA_END
};

static const u8 sDrumNoiseDataKick2[] =
{
    0x20, 0xA8, 0x6B, 
    0x20, 0x71, 0x00,
    NOISE_DATA_END
};

// Groups the separate samples into sample groups
static const u8 *const sDrumkitData0[] =
{
    sDrumNoiseDataDrum00,
    sDrumNoiseDataSnare1,
    sDrumNoiseDataSnare2,
    sDrumNoiseDataSnare3,
    sDrumNoiseDataSnare4,
    sDrumNoiseDataDrum05,
    sDrumNoiseDataTriangle1,
    sDrumNoiseDataTriangle2,
    sDrumNoiseDataHiHat1,
    sDrumNoiseDataSnare5,
    sDrumNoiseDataSnare6,
    sDrumNoiseDataSnare7,
    sDrumNoiseDataHiHat2
};

static const u8 *const sDrumkitData1[] =
{
    sDrumNoiseDataDrum00,
    sDrumNoiseDataHiHat1,
    sDrumNoiseDataSnare5,
    sDrumNoiseDataSnare6,
    sDrumNoiseDataSnare7,
    sDrumNoiseDataHiHat2,
    sDrumNoiseDataHiHat3,
    sDrumNoiseDataSnare8,
    sDrumNoiseDataTriangle3,
    sDrumNoiseDataTriangle4,
    sDrumNoiseDataSnare9,
    sDrumNoiseDataSnare10,
    sDrumNoiseDataSnare11
};

static const u8 *const sDrumkitData2[] =
{
    sDrumNoiseDataDrum00,
    sDrumNoiseDataSnare1,
    sDrumNoiseDataSnare9,
    sDrumNoiseDataSnare10,
    sDrumNoiseDataSnare11,
    sDrumNoiseDataDrum05,
    sDrumNoiseDataTriangle1,
    sDrumNoiseDataTriangle2,
    sDrumNoiseDataHiHat1,
    sDrumNoiseDataSnare5,
    sDrumNoiseDataSnare6,
    sDrumNoiseDataSnare7,
    sDrumNoiseDataHiHat2
};

static const u8 *const sDrumkitData3[] =
{
    sDrumNoiseDataDrum21,
    sDrumNoiseDataSnare12,
    sDrumNoiseDataSnare13,
    sDrumNoiseDataSnare14,
    sDrumNoiseDataKick1,
    sDrumNoiseDataTriangle5,
    sDrumNoiseDataDrum20,
    sDrumNoiseDataDrum27,
    sDrumNoiseDataDrum28,
    sDrumNoiseDataDrum29,
    sDrumNoiseDataDrum21,
    sDrumNoiseDataKick2,
    sDrumNoiseDataCrash2
};

static const u8 *const sDrumkitData4[] =
{
    sDrumNoiseDataDrum21,
    sDrumNoiseDataDrum20,
    sDrumNoiseDataSnare13,
    sDrumNoiseDataSnare14,
    sDrumNoiseDataKick1,
    sDrumNoiseDataDrum33,
    sDrumNoiseDataTriangle5,
    sDrumNoiseDataDrum35,
    sDrumNoiseDataDrum31,
    sDrumNoiseDataDrum32,
    sDrumNoiseDataDrum36,
    sDrumNoiseDataKick2,
    sDrumNoiseDataCrash1
};

static const u8 *const sDrumkitData5[] =
{
    sDrumNoiseDataDrum00,
    sDrumNoiseDataSnare9,
    sDrumNoiseDataSnare10,
    sDrumNoiseDataSnare11,
    sDrumNoiseDataDrum27,
    sDrumNoiseDataDrum28,
    sDrumNoiseDataDrum29,
    sDrumNoiseDataDrum05,
    sDrumNoiseDataTriangle1,
    sDrumNoiseDataCrash1,
    sDrumNoiseDataSnare14,
    sDrumNoiseDataSnare13,
    sDrumNoiseDataKick2
};

// A simple table pointing to each of the sample groups
static const u8 *const *const sDrumkitDataTable[] =
{
    sDrumkitData0,
    sDrumkitData1,
    sDrumkitData2,
    sDrumkitData3,
    sDrumkitData4,
    sDrumkitData5
};
