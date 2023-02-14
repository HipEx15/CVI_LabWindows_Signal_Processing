/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  FREQ_PANEL                       1       /* callback function: OnFreq */
#define  FREQ_PANEL_WINDOW_TYPE           2       /* control type: binary, callback function: (none) */
#define  FREQ_PANEL_SWITCHPANELS          3       /* control type: binary, callback function: OnPanels */
#define  FREQ_PANEL_FREQUENCY_CHEB_23     4       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FREQUENCY_CHEB_13     5       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FREQUENCY_RAW_SIG     6       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_WINDOW_CHEB_23        7       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_WINDOW_CHEB_13        8       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_WINDOW_RAW_SIG        9       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_SELECTION             10      /* control type: ring, callback function: GraphSelectionFreq */
#define  FREQ_PANEL_STOP                  11      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_FREQUENCY_PEAK        12      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_NUMBER_OF_WINDOWS     13      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_CURRENT_WINDOW        14      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_POWER_PEAK            15      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_START                 16      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_NEXT                  17      /* control type: command, callback function: OnNextFreq */
#define  FREQ_PANEL_PREV                  18      /* control type: command, callback function: OnPrevFreq */
#define  FREQ_PANEL_NUMBER_POINTS         19      /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_ORDER                 20      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_RIPPLE                21      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_BAND_ATTEND           22      /* control type: numeric, callback function: (none) */

#define  WAVE_PANEL                       2       /* callback function: OnWave */
#define  WAVE_PANEL_SAVE_BUTTON           2       /* control type: command, callback function: OnSave */
#define  WAVE_PANEL_LOADBUTTON            3       /* control type: command, callback function: OnLoad */
#define  WAVE_PANEL_LED                   4       /* control type: LED, callback function: (none) */
#define  WAVE_PANEL_DISPLAY_BOTH          5       /* control type: binary, callback function: (none) */
#define  WAVE_PANEL_SWITCHPANELS          6       /* control type: binary, callback function: OnPanels */
#define  WAVE_PANEL_POWERSWITCH           7       /* control type: binary, callback function: OnPower */
#define  WAVE_PANEL_GRAPH_2               8       /* control type: graph, callback function: (none) */
#define  WAVE_PANEL_GRAPH                 9       /* control type: graph, callback function: (none) */
#define  WAVE_PANEL_GRAPH_HISTOGRAM       10      /* control type: graph, callback function: (none) */
#define  WAVE_PANEL_ZERO                  11      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MEDIAN                12      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MEAN                  13      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_DISPERSION            14      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MIN_INDEX             15      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MAX_INDEX             16      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MINIM                 17      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_STOP                  18      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_START                 19      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MAXIM                 20      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_NEXT                  21      /* control type: command, callback function: OnNext */
#define  WAVE_PANEL_APPLY                 22      /* control type: command, callback function: OnApply */
#define  WAVE_PANEL_GEN_ENVELOPE          23      /* control type: command, callback function: OnEnvelope */
#define  WAVE_PANEL_GEN_DERIVATIVE        24      /* control type: command, callback function: OnDev */
#define  WAVE_PANEL_PREV                  25      /* control type: command, callback function: OnPrev */
#define  WAVE_PANEL_ALPHA                 26      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MEDIAN_TYPE           27      /* control type: ring, callback function: (none) */
#define  WAVE_PANEL_FILTERS               28      /* control type: ring, callback function: (none) */
#define  WAVE_PANEL_SELECTION             29      /* control type: ring, callback function: GraphSelection */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK GraphSelection(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphSelectionFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnApply(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnEnvelope(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFreq(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNext(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNextFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanels(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPower(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrevFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWave(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif