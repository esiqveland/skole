/*****************************************************************************
 * 
 * Øving 2 UCSysDes
 *
 * Headerfil
 *
 *****************************************************************************/

#ifndef OEVING_2_H /* vanlig måte å unngå headerrekursjon på */
#define OEVING_2_H

#include <avr32/ap7000.h> /* inkludere Atmel-headerfil */
#include <sys/interrupts.h>
#include <stdlib.h>

/* legg andre "includes" her, sammen med "defines" og prototyper */
#define ABDAC_INT_LEVEL 0
#define BUTTONS_INT_LEVEL 0

#define HALF 46874/2
/* notes */
#define H3 0
#define C4 1
#define Ciss4 2
#define D4 3
#define E4 4
#define F4 5
#define Fiss4 6
#define G4 7
#define Giss4 8
#define A4 9
#define H4 10
#define C5 11
#define Ciss5 12
#define D5 13

#define NUMTONES 14

short** MyTones; 

typedef struct {
//    short* tone;
    short tone;
    int size;
    int duration;
} Note;

typedef struct {
    int length; /* number of notes in song */
    Note* notes;
    //Note notes[];
} Song;


/* prototyper */
int main (int argc, char *argv[]);
static void initIntc(void);
static void initButtons(void);
static void initLeds(void);
static void initAudio(void);
static void initHardware (void);

static void button_isr(void);
static void abdac_isr(void);

void playSong( Song* s );
void storeSongs( void );

/*
short AtoneSize = 106;
short Atone[106] = { 1941, 3875, 5795, 7696, 9569, 11409, 13208, 14961, 16662, 18304, 19882, 21390, 22823, 24175, 25443, 26621, 27706, 28694, 29581, 30363, 31040, 31607, 32063, 32406, 32636, 32751, 32751, 32636, 32406, 32063, 31607, 31040, 30363, 29581, 28694, 27706, 26621, 25443, 24175, 22823, 21390, 19882, 18304, 16662, 14961, 13208, 11409, 9569, 7696, 5795, 3875, 1941, 0, -1941, -3875, -5795, -7696, -9569, -11409, -13208, -14961, -16662, -18304, -19882, -21390, -22823, -24175, -25443, -26621, -27706, -28694, -29581, -30363, -31040, -31607, -32063, -32406, -32636, -32751, -32751, -32636, -32406, -32063, -31607, -31040, -30363, -29581, -28694, -27706, -26621, -25443, -24175, -22823, -21390, -19882, -18304, -16662, -14961, -13208, -11409, -9569, -7696, -5795, -3875, -1941, 0  } ;
*/

short HlowSize = 190;
short Hlowtone[190] = { 1083, 2165, 3245, 4321, 5393, 6458, 7517, 8567, 9608, 10639, 11657, 12664, 13656, 14633, 15594, 16539, 17465, 18372, 19259, 20125, 20969, 21790, 22587, 23359, 24106, 24827, 25520, 26186, 26823, 27430, 28008, 28555, 29070, 29554, 30006, 30425, 30810, 31162, 31480, 31763, 32011, 32225, 32403, 32546, 32654, 32725, 32761, 32761, 32725, 32654, 32546, 32403, 32225, 32011, 31763, 31480, 31162, 30810, 30425, 30006, 29554, 29070, 28555, 28008, 27430, 26823, 26186, 25520, 24827, 24106, 23359, 22587, 21790, 20969, 20125, 19259, 18372, 17465, 16539, 15594, 14633, 13656, 12664, 11657, 10639, 9608, 8567, 7517, 6458, 5393, 4321, 3245, 2165, 1083, 0, -1083, -2165, -3245, -4321, -5393, -6458, -7517, -8567, -9608, -10639, -11657, -12664, -13656, -14633, -15594, -16539, -17465, -18372, -19259, -20125, -20969, -21790, -22587, -23359, -24106, -24827, -25520, -26186, -26823, -27430, -28008, -28555, -29070, -29554, -30006, -30425, -30810, -31162, -31480, -31763, -32011, -32225, -32403, -32546, -32654, -32725, -32761, -32761, -32725, -32654, -32546, -32403, -32225, -32011, -31763, -31480, -31162, -30810, -30425, -30006, -29554, -29070, -28555, -28008, -27430, -26823, -26186, -25520, -24827, -24106, -23359, -22587, -21790, -20969, -20125, -19259, -18372, -17465, -16539, -15594, -14633, -13656, -12664, -11657, -10639, -9608, -8567, -7517, -6458, -5393, -4321, -3245, -2165, -1083, 0  } ;

short CtoneSize = 179;
short Ctone[179] = { 1149, 2298, 3444, 4585, 5721, 6849, 7970, 9080, 10179, 11266, 12339, 13397, 14438, 15461, 16465, 17449, 18412, 19352, 20268, 21159, 22024, 22862, 23671, 24452, 25202, 25921, 26609, 27263, 27884, 28471, 29023, 29538, 30018, 30460, 30865, 31232, 31561, 31850, 32100, 32311, 32482, 32613, 32704, 32754, 32764, 32734, 32663, 32553, 32402, 32211, 31980, 31710, 31401, 31053, 30668, 30244, 29783, 29285, 28751, 28182, 27578, 26940, 26269, 25566, 24831, 24065, 23270, 22446, 21595, 20716, 19813, 18885, 17933, 16960, 15966, 14952, 13919, 12870, 11804, 10724, 9631, 8526, 7411, 6286, 5154, 4015, 2871, 1724, 575, -575, -1724, -2871, -4015, -5154, -6286, -7411, -8526, -9631, -10724, -11804, -12870, -13919, -14952, -15966, -16960, -17933, -18885, -19813, -20716, -21595, -22446, -23270, -24065, -24831, -25566, -26269, -26940, -27578, -28182, -28751, -29285, -29783, -30244, -30668, -31053, -31401, -31710, -31980, -32211, -32402, -32553, -32663, -32734, -32764, -32754, -32704, -32613, -32482, -32311, -32100, -31850, -31561, -31232, -30865, -30460, -30018, -29538, -29023, -28471, -27884, -27263, -26609, -25921, -25202, -24452, -23671, -22862, -22024, -21159, -20268, -19352, -18412, -17449, -16465, -15461, -14438, -13397, -12339, -11266, -10179, -9080, -7970, -6849, -5721, -4585, -3444, -2298, -1149, 0  } ;

short DtoneSize = 159;
short Dtone[159] = { 1294, 2586, 3875, 5157, 6432, 7696, 8948, 10186, 11409, 12613, 13798, 14961, 16101, 17216, 18304, 19363, 20393, 21390, 22354, 23283, 24175, 25030, 25846, 26621, 27355, 28046, 28694, 29296, 29853, 30363, 30826, 31241, 31607, 31923, 32190, 32406, 32572, 32687, 32751, 32764, 32726, 32636, 32496, 32304, 32063, 31771, 31430, 31040, 30601, 30114, 29581, 29001, 28376, 27706, 26994, 26239, 25443, 24608, 23734, 22823, 21876, 20895, 19882, 18837, 17764, 16662, 15534, 14383, 13208, 12013, 10800, 9569, 8324, 7065, 5795, 4517, 3231, 1941, 647, -647, -1941, -3231, -4517, -5795, -7065, -8324, -9569, -10800, -12013, -13208, -14383, -15534, -16662, -17764, -18837, -19882, -20895, -21876, -22823, -23734, -24608, -25443, -26239, -26994, -27706, -28376, -29001, -29581, -30114, -30601, -31040, -31430, -31771, -32063, -32304, -32496, -32636, -32726, -32764, -32751, -32687, -32572, -32406, -32190, -31923, -31607, -31241, -30826, -30363, -29853, -29296, -28694, -28046, -27355, -26621, -25846, -25030, -24175, -23283, -22354, -21390, -20393, -19363, -18304, -17216, -16101, -14961, -13798, -12613, -11409, -10186, -8948, -7696, -6432, -5157, -3875, -2586, -1294, 0  } ;

//short EtoneSize = 71;
//short Etone[71] = { 2895, 5769, 8597, 11357, 14029, 16591, 19024, 21307, 23423, 25357, 27091, 28614, 29913, 30978, 31800, 32373, 32693, 32757, 32565, 32118, 31420, 30475, 29292, 27880, 26250, 24414, 22387, 20185, 17825, 15325, 12706, 9987, 7190, 4336, 1449, -1449, -4336, -7190, -9987, -12706, -15325, -17825, -20185, -22387, -24414, -26250, -27880, -29292, -30475, -31420, -32118, -32565, -32757, -32693, -32373, -31800, -30978, -29913, -28614, -27091, -25357, -23423, -21307, -19024, -16591, -14029, -11357, -8597, -5769, -2895, 0  } ;
short EtoneSize = 142;
short Etone[142] = { 1449, 2895, 4336, 5769, 7190, 8597, 9987, 11357, 12706, 14029, 15325, 16591, 17825, 19024, 20185, 21307, 22387, 23423, 24414, 25357, 26250, 27091, 27880, 28614, 29292, 29913, 30475, 30978, 31420, 31800, 32118, 32373, 32565, 32693, 32757, 32757, 32693, 32565, 32373, 32118, 31800, 31420, 30978, 30475, 29913, 29292, 28614, 27880, 27091, 26250, 25357, 24414, 23423, 22387, 21307, 20185, 19024, 17825, 16591, 15325, 14029, 12706, 11357, 9987, 8597, 7190, 5769, 4336, 2895, 1449, 0, -1449, -2895, -4336, -5769, -7190, -8597, -9987, -11357, -12706, -14029, -15325, -16591, -17825, -19024, -20185, -21307, -22387, -23423, -24414, -25357, -26250, -27091, -27880, -28614, -29292, -29913, -30475, -30978, -31420, -31800, -32118, -32373, -32565, -32693, -32757, -32757, -32693, -32565, -32373, -32118, -31800, -31420, -30978, -30475, -29913, -29292, -28614, -27880, -27091, -26250, -25357, -24414, -23423, -22387, -21307, -20185, -19024, -17825, -16591, -15325, -14029, -12706, -11357, -9987, -8597, -7190, -5769, -4336, -2895, -1449, 0  } ;

//short FtoneSize = 67;
//short Ftone[67] = { 3068, 6109, 9097, 12004, 14806, 17478, 19997, 22340, 24486, 26417, 28116, 29568, 30760, 31682, 32325, 32684, 32756, 32541, 32039, 31255, 30197, 28874, 27297, 25480, 23439, 21192, 18758, 16160, 13420, 10562, 7611, 4593, 1535, -1535, -4593, -7611, -10562, -13420, -16160, -18758, -21192, -23439, -25480, -27297, -28874, -30197, -31255, -32039, -32541, -32756, -32684, -32325, -31682, -30760, -29568, -28116, -26417, -24486, -22340, -19997, -17478, -14806, -12004, -9097, -6109, -3068, 0  } ;

short FtoneSize = 134;
short Ftone[134] = { 1535, 3068, 4593, 6109, 7611, 9097, 10562, 12004, 13420, 14806, 16160, 17478, 18758, 19997, 21192, 22340, 23439, 24486, 25480, 26417, 27297, 28116, 28874, 29568, 30197, 30760, 31255, 31682, 32039, 32325, 32541, 32684, 32756, 32756, 32684, 32541, 32325, 32039, 31682, 31255, 30760, 30197, 29568, 28874, 28116, 27297, 26417, 25480, 24486, 23439, 22340, 21192, 19997, 18758, 17478, 16160, 14806, 13420, 12004, 10562, 9097, 7611, 6109, 4593, 3068, 1535, 0, -1535, -3068, -4593, -6109, -7611, -9097, -10562, -12004, -13420, -14806, -16160, -17478, -18758, -19997, -21192, -22340, -23439, -24486, -25480, -26417, -27297, -28116, -28874, -29568, -30197, -30760, -31255, -31682, -32039, -32325, -32541, -32684, -32756, -32756, -32684, -32541, -32325, -32039, -31682, -31255, -30760, -30197, -29568, -28874, -28116, -27297, -26417, -25480, -24486, -23439, -22340, -21192, -19997, -18758, -17478, -16160, -14806, -13420, -12004, -10562, -9097, -7611, -6109, -4593, -3068, -1535, 0  } ;

short FissSize = 127;
short Fisstone[127] = { 1620, 3236, 4845, 6442, 8022, 9584, 11121, 12632, 14112, 15557, 16964, 18329, 19650, 20923, 22144, 23311, 24422, 25472, 26460, 27383, 28240, 29027, 29743, 30386, 30955, 31449, 31865, 32203, 32463, 32643, 32743, 32763, 32703, 32563, 32343, 32044, 31666, 31212, 30680, 30074, 29394, 28642, 27820, 26930, 25974, 24954, 23874, 22735, 21540, 20293, 18996, 17652, 16265, 14839, 13376, 11880, 10356, 8806, 7234, 5645, 4042, 2429, 810, -810, -2429, -4042, -5645, -7234, -8806, -10356, -11880, -13376, -14839, -16265, -17652, -18996, -20293, -21540, -22735, -23874, -24954, -25974, -26930, -27820, -28642, -29394, -30074, -30680, -31212, -31666, -32044, -32343, -32563, -32703, -32763, -32743, -32643, -32463, -32203, -31865, -31449, -30955, -30386, -29743, -29027, -28240, -27383, -26460, -25472, -24422, -23311, -22144, -20923, -19650, -18329, -16964, -15557, -14112, -12632, -11121, -9584, -8022, -6442, -4845, -3236, -1620, 0  } ;

//short GtoneSize = 59;
//short Gtone[59] = { 3482, 6926, 10291, 13539, 16634, 19540, 22225, 24658, 26812, 28662, 30187, 31370, 32198, 32661, 32754, 32476, 31829, 30822, 29466, 27776, 25772, 23475, 20912, 18113, 15108, 11932, 8620, 5211, 1743, -1743, -5211, -8620, -11932, -15108, -18113, -20912, -23475, -25772, -27776, -29466, -30822, -31829, -32476, -32754, -32661, -32198, -31370, -30187, -28662, -26812, -24658, -22225, -19540, -16634, -13539, -10291, -6926, -3482, 0  } ;

short GtoneSize = 119;
short Gtone[119] = { 1729, 3453, 5168, 6868, 8550, 10207, 11836, 13432, 14990, 16507, 17978, 19399, 20765, 22074, 23321, 24503, 25617, 26659, 27628, 28519, 29330, 30060, 30706, 31267, 31740, 32125, 32421, 32626, 32740, 32763, 32694, 32535, 32284, 31944, 31515, 30998, 30394, 29706, 28935, 28083, 27153, 26147, 25069, 23920, 22705, 21427, 20089, 18695, 17249, 15754, 14216, 12638, 11025, 9382, 7712, 6020, 4312, 2592, 864, -864, -2592, -4312, -6020, -7712, -9382, -11025, -12638, -14216, -15754, -17249, -18695, -20089, -21427, -22705, -23920, -25069, -26147, -27153, -28083, -28935, -29706, -30394, -30998, -31515, -31944, -32284, -32535, -32694, -32763, -32740, -32626, -32421, -32125, -31740, -31267, -30706, -30060, -29330, -28519, -27628, -26659, -25617, -24503, -23321, -22074, -20765, -19399, -17978, -16507, -14990, -13432, -11836, -10207, -8550, -6868, -5168, -3453, -1729, 0  } ;

short AtoneSize = 106;
short Atone[106] = { 1941, 3875, 5795, 7696, 9569, 11409, 13208, 14961, 16662, 18304, 19882, 21390, 22823, 24175, 25443, 26621, 27706, 28694, 29581, 30363, 31040, 31607, 32063, 32406, 32636, 32751, 32751, 32636, 32406, 32063, 31607, 31040, 30363, 29581, 28694, 27706, 26621, 25443, 24175, 22823, 21390, 19882, 18304, 16662, 14961, 13208, 11409, 9569, 7696, 5795, 3875, 1941, 0, -1941, -3875, -5795, -7696, -9569, -11409, -13208, -14961, -16662, -18304, -19882, -21390, -22823, -24175, -25443, -26621, -27706, -28694, -29581, -30363, -31040, -31607, -32063, -32406, -32636, -32751, -32751, -32636, -32406, -32063, -31607, -31040, -30363, -29581, -28694, -27706, -26621, -25443, -24175, -22823, -21390, -19882, -18304, -16662, -14961, -13208, -11409, -9569, -7696, -5795, -3875, -1941, 0  } ;

//short AtoneSize = 53;
//short Atone[53] = { 3875, 7696, 11409, 14961, 18304, 21390, 24175, 26621, 28694, 30363, 31607, 32406, 32751, 32636, 32063, 31040, 29581, 27706, 25443, 22823, 19882, 16662, 13208, 9569, 5795, 1941, -1941, -5795, -9569, -13208, -16662, -19882, -22823, -25443, -27706, -29581, -31040, -32063, -32636, -32751, -32406, -31607, -30363, -28694, -26621, -24175, -21390, -18304, -14961, -11409, -7696, -3875, 0  } ;

//short HtoneSize = 47;
//short Htone[47] = { 4367, 8656, 12791, 16698, 20306, 23552, 26378, 28734, 30576, 31873, 32601, 32747, 32309, 31294, 29721, 27618, 25021, 21978, 18543, 14777, 10748, 6526, 2188, -2188, -6526, -10748, -14777, -18543, -21978, -25021, -27618, -29721, -31294, -32309, -32747, -32601, -31873, -30576, -28734, -26378, -23552, -20306, -16698, -12791, -8656, -4367, 0  } ;

short HtoneSize = 95;
short Htone[95] = { 2165, 4321, 6458, 8567, 10639, 12664, 14633, 16539, 18372, 20125, 21790, 23359, 24827, 26186, 27430, 28555, 29554, 30425, 31162, 31763, 32225, 32546, 32725, 32761, 32654, 32403, 32011, 31480, 30810, 30006, 29070, 28008, 26823, 25520, 24106, 22587, 20969, 19259, 17465, 15594, 13656, 11657, 9608, 7517, 5393, 3245, 1083, -1083, -3245, -5393, -7517, -9608, -11657, -13656, -15594, -17465, -19259, -20969, -22587, -24106, -25520, -26823, -28008, -29070, -30006, -30810, -31480, -32011, -32403, -32654, -32761, -32725, -32546, -32225, -31763, -31162, -30425, -29554, -28555, -27430, -26186, -24827, -23359, -21790, -20125, -18372, -16539, -14633, -12664, -10639, -8567, -6458, -4321, -2165, 0  } ;

short ChoySize = 89;
short Choy[89] =  { 2311, 4611, 6887, 9130, 11327, 13467, 15541, 17537, 19446, 21257, 22963, 24555, 26024, 27363, 28566, 29627, 30541, 31302, 31907, 32353, 32638, 32760, 32720, 32516, 32150, 31624, 30940, 30103, 29115, 27982, 26710, 25305, 23774, 22124, 20364, 18503, 16549, 14513, 12405, 10235, 8014, 5753, 3463, 1156, -1156, -3463, -5753, -8014, -10235, -12405, -14513, -16549, -18503, -20364, -22124, -23774, -25305, -26710, -27982, -29115, -30103, -30940, -31624, -32150, -32516, -32720, -32760, -32638, -32353, -31907, -31302, -30541, -29627, -28566, -27363, -26024, -24555, -22963, -21257, -19446, -17537, -15541, -13467, -11327, -9130, -6887, -4611, -2311, 0  } ;

short CisshoySize = 84;
short Cisshoy[84] = { 2448, 4883, 7291, 9657, 11970, 14216, 16382, 18457, 20429, 22286, 24019, 25617, 27072, 28376, 29521, 30501, 31310, 31944, 32400, 32674, 32766, 32674, 32400, 31944, 31310, 30501, 29521, 28376, 27072, 25617, 24019, 22286, 20429, 18457, 16383, 14216, 11970, 9657, 7291, 4883, 2448, 0, -2448, -4883, -7291, -9657, -11970, -14216, -16382, -18457, -20429, -22286, -24019, -25617, -27072, -28376, -29521, -30501, -31310, -31944, -32400, -32674, -32766, -32674, -32400, -31944, -31310, -30501, -29521, -28376, -27072, -25617, -24019, -22286, -20429, -18457, -16383, -14216, -11970, -9657, -7291, -4883, -2448, 0  } ;

short DhoySize = 79;
short Dhoy[79] = { 2603, 5190, 7744, 10249, 12689, 15049, 17314, 19469, 21501, 23398, 25146, 26736, 28156, 29398, 30455, 31319, 31985, 32449, 32707, 32759, 32604, 32242, 31677, 30911, 29950, 28800, 27468, 25961, 24291, 22467, 20501, 18406, 16194, 13880, 11478, 9003, 6472, 3899, 1302, -1302, -3899, -6472, -9003, -11478, -13880, -16194, -18406, -20501, -22467, -24291, -25961, -27468, -28800, -29950, -30911, -31677, -32242, -32604, -32759, -32707, -32449, -31985, -31319, -30455, -29398, -28156, -26736, -25146, -23398, -21501, -19469, -17314, -15049, -12689, -10249, -7744, -5190, -2603, 0  } ;


//short CsharpSize = 44;
//short Csharp[44] = { 4663, 9231, 13611, 17714, 21457, 24762, 27564, 29805, 31438, 32432, 32766, 32432, 31438, 29805, 27564, 24762, 21457, 17714, 13611, 9231, 4663, 0, -4663, -9231, -13611, -17714, -21457, -24762, -27564, -29805, -31438, -32432, -32766, -32432, -31438, -29805, -27564, -24762, -21457, -17714, -13611, -9231, -4663, 0  } ;

/*
Song hp = { 60,
    {
	{ H3, HlowSize, HALF*2 },

        { E4, EtoneSize, HALF*3 },
        { G4, GtoneSize, HALF },
        { F4, FtoneSize, HALF*2 },

        { E4, EtoneSize, HALF*4 },
        { H4, HtoneSize, HALF*2 },

        { A4, AtoneSize, HALF*6 },

        { Fiss4, FissSize, HALF*6 },

        { E4, EtoneSize, HALF*3 },
        { G4, GtoneSize, HALF },
        { Fiss4, FissSize, HALF*2 },

        { D4, DtoneSize, HALF*4 },
        { Fiss4, FissSize, HALF*2 },

        { H3, HlowSize, HALF*10 },
        { H3, HlowSize, HALF*2 },

        { E4, EtoneSize, HALF*3 },
        { G4, GtoneSize, HALF },
        { Fiss4, FissSize, HALF*2 },

        { E4, EtoneSize, HALF*4 },
        { H4, HtoneSize, HALF*2 },

        { D5, DhoySize, HALF*4 },
        { Ciss5, CisshoySize, HALF*2 },

        { C5, ChoySize, HALF*4 },
        { G4, GtoneSize, HALF*2 },    // giss egentlig

        { C5, ChoySize, HALF*3 },
        { H4, HtoneSize, HALF },
        { H4, HtoneSize, HALF*2 },    // egentlig b for h (b for b?) // 27

        { H3, HlowSize, HALF*4 },  // egentlig b for lav h // 28
        { G4, GtoneSize, HALF*2 },

        { E4, EtoneSize, HALF*10 },
        { G4, GtoneSize, HALF*2 },

        { H4, HtoneSize, HALF*4 },
        { G4, GtoneSize, HALF*2 },

        { H4, HtoneSize, HALF*4 },
        { G4, GtoneSize, HALF*2 },

        { C5, ChoySize, HALF*4 },
        { H4, HtoneSize, HALF*2 },

        { H4, HtoneSize, HALF*4 },    // egentlig b for h (b for b?)
        { Fiss4, FissSize, HALF*2 },

        { G4, GtoneSize, HALF*3 },
        { A4, AtoneSize, HALF },
        { H4, HtoneSize, HALF*2 },    // egentlig b for h (b for b?)

        { H3, HlowSize, HALF*4 },  // egentlig b for lav h
        { C4 CtoneSize, HALF*2 },

        { H3, HlowSize, HALF*10 },
        { G4, GtoneSize, HALF*2 },    // 46

        { H4, HtoneSize, HALF*4 },
        { G4, GtoneSize, HALF*2 },

        { H4, HtoneSize, HALF*4 },
        { G4, GtoneSize, HALF*2 },

        { D5, DhoySize, HALF*4 },
        { Ciss5, CisshoySize, HALF*2 },

        { C5, ChoySize, HALF*4 },
        { G4, GtoneSize, HALF*2 },    // giss egentlig

        { C5, ChoySize, HALF*3 },
        { H4, HtoneSize, HALF },
        { H4, HtoneSize, HALF*2 },    // egentlig b for h (b for b?) // 27

        { H3, HlowSize, HALF*4 },  // egentlig b for lav h // 28
        { G4, GtoneSize, HALF*2 },

        { E4, EtoneSize, HALF*12 }
    }
};
*/

/*
test = { 12,
    {
        { C4, CtoneSize, HALF },
        { D4, DtoneSize, HALF },
        { E4, EtoneSize, HALF },
        { C4, CtoneSize, HALF },
        { D4, DtoneSize, HALF },
        { E4, EtoneSize, HALF },
        { D4, DtoneSize, HALF },
        { D4, DtoneSize, HALF },
        { D4, DtoneSize, HALF },
        { D4, DtoneSize, HALF },
        { C4, CtoneSize, HALF*2 },
        { C4, CtoneSize, HALF*2 }
    }
};
*/
#endif

