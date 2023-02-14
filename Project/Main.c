#include <advanlys.h>
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "WAV_Interface.h"
#include "toolbox.h"

// Constants
#define SAMPLE_RATE		0
#define NPOINTS			1

// Global variables
int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
int npoints = 0;
int MedVal = 16;
double alpha = 0.01;
double sampleRate = 0.0;
static double *waveData = 0;
static double *anvelopa = 0;
static double *signalSeconds = 0;
static double *filters = 0;
static double *filterRes = 0;
static double *dev = 0;
static double *anvRes = 0;
static double *rawWin = 0;
static double* window_raw = 0;
static double* result_raw = 0;
static double* window_cb1 = 0;
static double* result_cb1 = 0;
static double* window_cb2 = 0;
static double* result_cb2 = 0;
static double* window_timer = 0;
static double* result_timer = 0;
static double* vectLocation_1 = 0;
static double* vectLocation_2 = 0;
static double* vectLocation_3 = 0;
static double* Cheb1 = 0;
static double* Cheb2 = 0;
static double* Cheb1_none = 0;
static double* Cheb2_none = 0;
static double* Window_np = 0;
static double* Result_np = 0;
WindowConst winConst;
static double *convertedSpectrum = 0; //vectorul ce contine spectrul semnalului convertit in volti
static double *autoSpectrum = 0; //spectrul de putere

// Panels
static int wavePanel = 0;
static int frequencyPanel = 0;

//Variables for functions
static int PowerMode = 0;
static int loading = 0;
static int Start = 0;
static int Stop = 1;
static int selection = 0;
static int BothGraphics = 0;
static int filterOn = 0;
static int med = 0, order = 0;
static int DevOnOff = 0;
static int FStart = 0;
static int FStop = 1;
static int Fselection = 0;
static int winType = 0;
static unsigned int NumberOfPoints = 1024;
static unsigned int Order = 5;
static double StopAttend = 40.0;
static double Ripple = 0.01;
const double a0 = (1.0-0.16)/2.0;
const double a1 = 1.0/2.0;
const double a2 = 0.16/2.0;

static double df=0.0; //pasul in domeniul frecventei
static double freqPeak=0.0; //valoarea maxima din spectrul de putere
static double powerPeak=0.0; //frecventa estimata pentru spectrum de putere
char unit[32]="V";  //voltage semnal

int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (wavePanel = LoadPanel (0, "WAV_Interface.uir", WAVE_PANEL));
	errChk (frequencyPanel = LoadPanel (0, "WAV_Interface.uir", FREQ_PANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (wavePanel));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (wavePanel > 0)
		DiscardPanel (wavePanel);
	return 0;
}

int CVICALLBACK OnWave (int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			// activarea butonului pentru inchiderea ferestrei
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnFreq (int panel, int event, void *callbackData,
						int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			// activarea butonului pentru inchiderea ferestrei
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnPower (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//setarea obiectelor de pe interfata ca fiind pornite/oprite, atunci cand actionam switchul de "Power"
			GetCtrlVal(wavePanel, WAVE_PANEL_POWERSWITCH, &PowerMode);
			SetCtrlVal(wavePanel, WAVE_PANEL_LED, PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_LOADBUTTON, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_GRAPH, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_GRAPH_HISTOGRAM, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MAXIM, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MINIM, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MEDIAN, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_ZERO, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_DISPERSION, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MEAN, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MAX_INDEX, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MIN_INDEX, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_GRAPH_2, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_PREV, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_NEXT, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_START, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_STOP, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel,	WAVE_PANEL_SWITCHPANELS, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_GEN_DERIVATIVE, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_GEN_ENVELOPE, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_DISPLAY_BOTH, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_SELECTION, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_FILTERS, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_ALPHA, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_MEDIAN_TYPE, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_APPLY, ATTR_DIMMED, !PowerMode);
			SetCtrlAttribute(wavePanel, WAVE_PANEL_SAVE_BUTTON, ATTR_DIMMED, !PowerMode);
			
			// in cazul setarii switch-ului pe "Off", toate obiectele de pe interfata vor reveni la valorile initiale
			if(PowerMode == 0)
			{
				SetCtrlAttribute(wavePanel, WAVE_PANEL_APPLY, ATTR_DIMMED, !PowerMode);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_SELECTION, ATTR_DIMMED, !PowerMode);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_FILTERS, ATTR_DIMMED, !PowerMode);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_ALPHA, ATTR_DIMMED, !PowerMode);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_MEDIAN_TYPE, ATTR_DIMMED, !PowerMode);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_SAVE_BUTTON, ATTR_DIMMED, !PowerMode);
				SetCtrlVal(wavePanel, WAVE_PANEL_SELECTION, 0);
				SetCtrlVal(wavePanel, WAVE_PANEL_FILTERS,  0);
				SetCtrlVal(wavePanel, WAVE_PANEL_MEDIAN_TYPE,  0);
				SetCtrlVal(wavePanel, WAVE_PANEL_ALPHA, 0.01);
				SetCtrlVal(wavePanel, WAVE_PANEL_START,  0);
				SetCtrlVal(wavePanel, WAVE_PANEL_STOP,  1);
				SetCtrlVal(wavePanel, WAVE_PANEL_MAXIM,  0.0);
				SetCtrlVal(wavePanel, WAVE_PANEL_MINIM, 0.0);
				SetCtrlVal(wavePanel, WAVE_PANEL_MEDIAN, 0.0);
				SetCtrlVal(wavePanel, WAVE_PANEL_ZERO, 0);
				SetCtrlVal(wavePanel, WAVE_PANEL_DISPERSION, 0.0);
				SetCtrlVal(wavePanel, WAVE_PANEL_MEAN, 0.0);
				SetCtrlVal(wavePanel, WAVE_PANEL_MAX_INDEX,  0);
				SetCtrlVal(wavePanel, WAVE_PANEL_MIN_INDEX, 0 );
				SetCtrlAttribute(wavePanel, WAVE_PANEL_GEN_DERIVATIVE, ATTR_DIMMED, !PowerMode);
				SetCtrlVal(wavePanel, WAVE_PANEL_GEN_ENVELOPE, 0);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_DISPLAY_BOTH, ATTR_DIMMED, !PowerMode);
				DeleteGraphPlot(wavePanel, WAVE_PANEL_GRAPH,-1,VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot(wavePanel, WAVE_PANEL_GRAPH_2,-1,VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot(wavePanel, WAVE_PANEL_GRAPH_HISTOGRAM,-1,VAL_IMMEDIATE_DRAW);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnLoad (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	double maxVal = 0.0;
	double minVal = 0.0;
	int maxIndex = 0;
	int minIndex = 0;
	double dispersion = 0.0;
	double mean = 0.0;
	double median = 0.0;
	int zeros = 0;
	int histogram[100];
	double axis[100];
	
	switch (event)
	{
		case EVENT_COMMIT:
			if(loading == 0)
			{
				loading = 1;
				SetCtrlAttribute(wavePanel, WAVE_PANEL_SELECTION, ATTR_DIMMED, !PowerMode);
				//executa script python pentru conversia unui fisierului .wav in .txt
				LaunchExecutable("python main.py");
				
				//incarc informatiile privind rata de esantionare si numarul de valori
				FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
				sampleRate = waveInfo[SAMPLE_RATE];
				npoints = waveInfo[NPOINTS];
				
				//alocare memorie pentru numarul de puncte
				waveData = (double *) calloc(npoints, sizeof(double));
				rawWin = (double*)calloc(npoints,sizeof(double));
				
				//incarcare din fisierul .txt in memorie (vector)
				FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
				//FileToArray("waveData.txt", rawWin, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
				Copy1D(waveData, npoints, rawWin);
				
				//anvelopa
				//LaunchExecutable("python anvelopa.py");
				//alocare memorie pentru numarul de puncte
				anvelopa = (double *) calloc(npoints, sizeof(double));  
				//incarcare din fisierul .txt in memorie (vector)
				FileToArray("anvelopa.txt", anvelopa, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
				
				//PlotY(panel, PANEL_GRAPH, matrix[Start], (Stop - Start) * sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, WAVE_PANEL_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				//calculare valoare minima si valoare maxima
				MaxMin1D(waveData,npoints,&maxVal,&maxIndex,&minVal,&minIndex);
				//calculare valoare medie
				Mean(waveData,npoints,&mean);
				//calculare dispersie
				StdDev(waveData, npoints, &mean, &dispersion);
				//calculare mediana
				Median(waveData, npoints, &median);
				//calculare zerouri
				for (int i=0; i < npoints-1; i++)
					if ( (waveData[i] <= 0 && waveData[i+1] >= 0) || (waveData[i] >= 0 && waveData[i+1] <= 0))
						zeros++;
				
				//setare valori
				SetCtrlVal(panel, WAVE_PANEL_MINIM, minVal);
				SetCtrlVal(panel, WAVE_PANEL_MAXIM, maxVal);
				SetCtrlVal(panel, WAVE_PANEL_MIN_INDEX, minIndex);
				SetCtrlVal(panel, WAVE_PANEL_MAX_INDEX, maxIndex);
				SetCtrlVal(panel, WAVE_PANEL_DISPERSION, dispersion);
				SetCtrlVal(panel, WAVE_PANEL_MEAN, mean);
				SetCtrlVal(panel, WAVE_PANEL_MEDIAN, median);
				SetCtrlVal(panel, WAVE_PANEL_ZERO, zeros);
				
				//afisare histograma
				DeleteGraphPlot (panel, WAVE_PANEL_GRAPH_HISTOGRAM, -1, VAL_DELAYED_DRAW);
				Histogram(waveData, npoints, minVal, maxVal, histogram, axis, 10);
				PlotXY (panel, WAVE_PANEL_GRAPH_HISTOGRAM, axis,  histogram, 10, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
				SetCtrlAttribute(wavePanel, WAVE_PANEL_GEN_ENVELOPE, ATTR_DIMMED, !loading);
				break;
			}
	}
	return 0;
}

// pentru a face trecerea dintre "paginile" semnalului, am considerat 2 variabile globale, "Start" si "Stop"
// ele modificandu-se atat cand parcurgem semnalul de la stanga la dreapta, dar si in sens invers

int CVICALLBACK OnPrev (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
				SetCtrlVal(wavePanel, WAVE_PANEL_START,  0);
			}
			else
			{
				GetCtrlVal(panel, WAVE_PANEL_STOP, &Stop);
				GetCtrlVal(panel, WAVE_PANEL_START, &Start);
				
				if(Stop > 1 && Start <= 6)
				{
					Start = Start - 1;
					Stop = Stop - 1;
					GraphSelection(panel, WAVE_PANEL_PREV,EVENT_COMMIT,0,0,0);
					SetCtrlVal(panel, WAVE_PANEL_START, Start);
					SetCtrlVal(panel, WAVE_PANEL_STOP, Stop);
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnNext (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
				SetCtrlVal(wavePanel, WAVE_PANEL_STOP,  1);
			}
			else
			{
				GetCtrlVal(panel, WAVE_PANEL_STOP, &Stop);
				GetCtrlVal(panel, WAVE_PANEL_START, &Start);
				
					
				if(Stop >= 0 && Start < 5)
				{
					Start = Start + 1;
					Stop = Stop + 1;
					GraphSelection(panel, WAVE_PANEL_PREV,EVENT_COMMIT,0,0,0);
					SetCtrlVal(panel, WAVE_PANEL_START, Start);
					SetCtrlVal(panel, WAVE_PANEL_STOP, Stop);
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK GraphSelection (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	double maxVal = 0.0;
	double minVal = 0.0;
	int maxIndex = 0;
	int minIndex = 0;
	double dispersion = 0.0;
	double mean = 0.0;
	double median = 0.0;
	int zeros = 0;
	int histogram[100];
	double axis[100];
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(wavePanel, WAVE_PANEL_SELECTION, &selection);
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
				SetCtrlVal(wavePanel, WAVE_PANEL_SELECTION, 0);
			}
			else
			{
				switch (selection)
				{
					// calculez si afisez valorile pe primul grafic al intregului semnal incarcat
					case 0:
						// acest if este folosit pentru a ma asigura ca nu se pot da click-uri gresite
						if(loading == 0)
							break;
						else
						{
							DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, WAVE_PANEL_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
							//calculare valoare minima si valoare maxima
							MaxMin1D(waveData,npoints,&maxVal,&maxIndex,&minVal,&minIndex);
							//calculare valoare medie
							Mean(waveData,npoints,&mean);
							//calculare dispersie
							StdDev(waveData, npoints, &mean, &dispersion);
							//calculare mediana
							Median(waveData, npoints, &median);
							//calculare zerouri
							for (int i=0; i < npoints-1; i++)
								if ( (waveData[i] < 0 && waveData[i+1] > 0) || (waveData[i] > 0 && waveData[i+1] < 0))
									zeros++;
							
							//setare valori
							SetCtrlVal(panel, WAVE_PANEL_MINIM, minVal);
							SetCtrlVal(panel, WAVE_PANEL_MAXIM, maxVal);
							SetCtrlVal(panel, WAVE_PANEL_MIN_INDEX, minIndex);
							SetCtrlVal(panel, WAVE_PANEL_MAX_INDEX, maxIndex);
							SetCtrlVal(panel, WAVE_PANEL_DISPERSION, dispersion);
							SetCtrlVal(panel, WAVE_PANEL_MEAN, mean);
							SetCtrlVal(panel, WAVE_PANEL_MEDIAN, median);
							SetCtrlVal(panel, WAVE_PANEL_ZERO, zeros);
							
							//afisare histograma
							DeleteGraphPlot (panel, WAVE_PANEL_GRAPH_HISTOGRAM, -1, VAL_DELAYED_DRAW);
							Histogram(waveData, npoints, minVal, maxVal, histogram, axis, 10);
							PlotXY (panel, WAVE_PANEL_GRAPH_HISTOGRAM, axis,  histogram, 10, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
						}
						break;
					// calculez si afisez valorile pe primul grafic al semnalului incarcat, insa doar pe bucati / "pagini"
					// trecerea intre acestea facandu-se cu ajutorul butoanelor de "Next" si "Prev"
					case 1:
						if(loading == 0)
							break;
						else
						{
							signalSeconds = waveData + (int)(Start * sampleRate);
							DeleteGraphPlot(wavePanel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, WAVE_PANEL_GRAPH, signalSeconds, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
							//calculare valoare minima si valoare maxima
							MaxMin1D(signalSeconds,sampleRate,&maxVal,&maxIndex,&minVal,&minIndex);
							//calculare valoare medie
							Mean(signalSeconds,sampleRate,&mean);
							//calculare dispersie
							StdDev(signalSeconds, sampleRate, &mean, &dispersion);
							//calculare mediana
							Median(signalSeconds, sampleRate, &median);
							//calculare zerouri
							for (int i=0; i < sampleRate-1; i++)
								if ( (signalSeconds[i] < 0 && signalSeconds[i+1] > 0) || (signalSeconds[i] > 0 &&signalSeconds[i+1] < 0))
									zeros++;
							
							//setare valori
							SetCtrlVal(panel, WAVE_PANEL_MINIM, minVal);
							SetCtrlVal(panel, WAVE_PANEL_MAXIM, maxVal);
							SetCtrlVal(panel, WAVE_PANEL_MIN_INDEX, minIndex);
							SetCtrlVal(panel, WAVE_PANEL_MAX_INDEX, maxIndex);
							SetCtrlVal(panel, WAVE_PANEL_DISPERSION, dispersion);
							SetCtrlVal(panel, WAVE_PANEL_MEAN, mean);
							SetCtrlVal(panel, WAVE_PANEL_MEDIAN, median);
							SetCtrlVal(panel, WAVE_PANEL_ZERO, zeros);
							
							//afisare histograma
							DeleteGraphPlot (panel, WAVE_PANEL_GRAPH_HISTOGRAM, -1, VAL_DELAYED_DRAW);
							Histogram(signalSeconds, sampleRate, minVal, maxVal, histogram, axis, 10);
							PlotXY (panel, WAVE_PANEL_GRAPH_HISTOGRAM, axis,  histogram, 10, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
						}
						break;
					// deblochez optiunea utilizatorului de a alege ce filtre doreste sa foloseasca
					case 2:

					GetCtrlVal(wavePanel, WAVE_PANEL_FILTERS, &filterOn);	
					GetCtrlVal(wavePanel, WAVE_PANEL_DISPLAY_BOTH, &BothGraphics);
					GetCtrlVal(panel,WAVE_PANEL_MEDIAN_TYPE, &MedVal);
					GetCtrlVal(panel,WAVE_PANEL_ALPHA, &alpha);
					
					if(filterOn == 1) // filtru mediere
					{	
						double sum = 0;
						
						if(med == 0)
						{
							filters = (double *) calloc(npoints, sizeof(double));
							for (int i = 0; i < npoints; i++)
							{
								for (int k = 0; k <= MedVal - 1; k++)
								{
									if ((i - k) < 0)
									{
										sum += 0;
									}
									else	
									{
										sum += waveData[i-k];	
									}
								}
								filters[i] = sum/MedVal;
								sum = 0;
								
							}
							med = 1;
							order = 0;
						}
					}
					else if(filterOn == 2) // filtru element de ordin 1
					{	
						if(order == 0)
						{
							filters = (double *) calloc(npoints, sizeof(double));
							filters[0] = waveData[0];
							for(int i = 1; i < npoints; i++)
							{
								filters[i] = (1-alpha)*filters[i-1] + alpha*waveData[i];
							}
							
							med = 0;
							order = 1;
						}
					}
					
					filterRes = filters + (int)(Start * sampleRate);
					signalSeconds = waveData + (int)(Start * sampleRate);	
					
					if(BothGraphics == 1)
					{
						DeleteGraphPlot(wavePanel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, WAVE_PANEL_GRAPH, signalSeconds, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					}
					else
					{
						DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, WAVE_PANEL_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					}
					
					DeleteGraphPlot (panel,WAVE_PANEL_GRAPH_2, -1, VAL_DELAYED_DRAW);
					PlotY(panel, WAVE_PANEL_GRAPH_2, filterRes ,sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					SetCtrlAttribute(wavePanel,WAVE_PANEL_GRAPH_2, ATTR_XAXIS_OFFSET, Start * sampleRate);
					break;
						
				}
			}
	}
	return 0;
}

int CVICALLBACK OnApply (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
			}
			else
			{
				med = 0;
				order = 0;
				realloc(filters, 0);
			}
			
			break;
	}
	return 0;
}


// calculez si afisez derivata semnalului in domeniul timp. Aceasta o afisez peste valorile deja filtrate (indiferent de filtrul ales) cu o alta culoare
int CVICALLBACK OnDev (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	GetCtrlVal(panel, WAVE_PANEL_GEN_DERIVATIVE, &DevOnOff);
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
			}
			else
			{
				if(DevOnOff == 0)
				{
					dev = (double *) calloc(sampleRate*(Stop - Start), sizeof(double));
					Difference(filterRes, sampleRate, 1, 0, 0, dev);
					PlotY(panel, WAVE_PANEL_GRAPH_2, dev, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);  
					free(dev);	
				}
			}
	}
	return 0;
}

// calcularea anvelopei si afisarea acesteia pe graficul 1. Totodata, am pregatit optiunea de a afisa aceeasi bucata/"pagina" concomitent.
// astfel, cand aceasta optiune este activata, utilizatorul va putea vizualiza semnalul initial, cu anvelopa calculata, impreuna cu un filtru ales
// peste care am suprapus si derivata acestuia in timp
int CVICALLBACK OnEnvelope (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
			}
			else
			{
				anvRes = anvelopa + (int)(Start * sampleRate);
				switch (selection)
				{
						
					case 0:
						PlotY(panel, WAVE_PANEL_GRAPH, anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE); 
						break;
					case 1:
						if(BothGraphics == 1)
							PlotY(panel, WAVE_PANEL_GRAPH, anvRes, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
						else
							PlotY(panel, WAVE_PANEL_GRAPH, anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE); 
						break;
					case 2:
						if(BothGraphics == 1)
							PlotY(panel, WAVE_PANEL_GRAPH, anvRes, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
						else
							PlotY(panel, WAVE_PANEL_GRAPH, anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE); 
						break;
				}
			}
	}
	return 0;
}

int CVICALLBACK OnPanels (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(loading == 0)
			{
				MessagePopup("ERROR !!!", "PRESS LOAD BUTTON");
				SetCtrlVal(wavePanel, WAVE_PANEL_SWITCHPANELS,  0);
			}
			else if(panel == wavePanel)
			{
				SetCtrlVal(frequencyPanel, FREQ_PANEL_SWITCHPANELS, 1);
				DisplayPanel(frequencyPanel);
				HidePanel(panel);
			}
			else
			{
				SetCtrlVal(wavePanel, WAVE_PANEL_SWITCHPANELS, 0);
				DisplayPanel(wavePanel);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnPrevFreq (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
				GetCtrlVal(panel, FREQ_PANEL_STOP, &FStop);
				GetCtrlVal(panel, FREQ_PANEL_START, &FStart);
				
				if(FStop > 1 && FStart <= 6)
				{
					FStart = FStart - 1;
					FStop = FStop - 1;
					GraphSelectionFreq(panel, FREQ_PANEL_PREV,EVENT_COMMIT,0,0,0);
					SetCtrlVal(panel, FREQ_PANEL_START, FStart);
					SetCtrlVal(panel, FREQ_PANEL_STOP, FStop);
				}
			
			break;
	}
	return 0;
}

int CVICALLBACK OnNextFreq (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
				GetCtrlVal(panel, FREQ_PANEL_STOP, &FStop);
				GetCtrlVal(panel, FREQ_PANEL_START, &FStart);
				
					
				if(FStop >= 0 && FStart < 5)
				{
					FStart = FStart + 1;
					FStop = FStop + 1;
					GraphSelectionFreq(panel, FREQ_PANEL_PREV,EVENT_COMMIT,0,0,0);
					SetCtrlVal(panel, FREQ_PANEL_START, FStart);
					SetCtrlVal(panel, FREQ_PANEL_STOP, FStop);
				}
			break;
	}
	return 0;
}

int CVICALLBACK GraphSelectionFreq (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{		
	switch (event)
	{
		case EVENT_COMMIT:
				double Result_np1[npoints/6],  Result_np2[npoints/6];
				GetCtrlVal(frequencyPanel, FREQ_PANEL_SELECTION, &Fselection);
				switch (Fselection)
				{
					case 0: //Option "None"
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 1);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 1);
						
						window_raw = (double*)calloc(npoints,sizeof(double));
						result_raw = (double*)calloc(npoints,sizeof(double));
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						LinEv1D(rawWin,npoints,0,1,window_raw);	   
							
						switch (winType)
						{
							case 0:
								//BlkHarrisWin(window_raw,npoints);
								for(int i=0; i < npoints - 1; ++i)
									window_raw[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints)) + a2 * cos((4.0 * PI * i)/(double)(npoints));
								break;
							case 1:
								TriWin(window_raw, npoints);
								break;
						}
						Mul1D(rawWin,window_raw,npoints,result_raw);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG, result_raw, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						
						
						window_cb1 = (double*)calloc(npoints,sizeof(double));
						result_cb1 = (double*)calloc(npoints,sizeof(double));
						Cheb1_none = (double*)calloc(npoints,sizeof(double));
						
						GetCtrlVal(panel, FREQ_PANEL_ORDER, &Order);
						GetCtrlVal(panel, FREQ_PANEL_RIPPLE, &Ripple);
						Ch_BPF(rawWin, npoints, sampleRate,  sampleRate/6, sampleRate * 2/6, Ripple, Order, Cheb1_none);
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						LinEv1D(Cheb1_none,npoints,0,1,window_cb1);	   
							
						switch (winType)
						{
							case 0:
								//BlkHarrisWin(window_cb1,npoints);
								for(int i=0; i < npoints - 1; ++i)
									window_cb1[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints)) + a2 * cos((4.0 * PI * i)/(double)(npoints));
								break;
							case 1:
								TriWin(window_cb1, npoints);
								break;
						}
						Mul1D(Cheb1_none,window_cb1,npoints,result_cb1);
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13, result_cb1, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						
						window_cb2 = (double*)calloc(npoints,sizeof(double));
						result_cb2 = (double*)calloc(npoints,sizeof(double));
						Cheb2_none = (double*)calloc(npoints,sizeof(double));
						
						GetCtrlVal(panel, FREQ_PANEL_ORDER, &Order);
						GetCtrlVal(panel, FREQ_PANEL_BAND_ATTEND, &StopAttend);
						InvCh_BPF(rawWin, npoints, sampleRate,  sampleRate/6, sampleRate * 2/6, StopAttend, Order, Cheb2_none);
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						LinEv1D(Cheb2_none,npoints,0,1,window_cb2);	   
							
						switch (winType)
						{
							case 0:
								//BlkHarrisWin(window_cb2,npoints);
								for(int i=0; i < npoints - 1; ++i)
									window_cb2[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints)) + a2 * cos((4.0 * PI * i)/(double)(npoints));
								break;
							case 1:
								TriWin(window_cb2, npoints);
								break;
						}
						Mul1D(Cheb2_none,window_cb2,npoints,result_cb2);
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23, result_cb2, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						
						realloc(window_raw, 0);
						realloc(result_raw, 0);
						
						realloc(window_cb1, 0);
						realloc(result_cb1, 0);
						realloc(Cheb1_none, 0);
						
						realloc(window_cb2, 0);
						realloc(result_cb2, 0);
						realloc(Cheb2_none, 0);
						
						break;
					case 1: //Option "All"
						//Ch_BPF(result, npoints, sampleRate, 
						//Ch_BPF(
						//printf("%lf", sampleRate);
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 0);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 0);
						
						//1
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						
						vectLocation_1 = rawWin + (int)(FStart * npoints/6);
						Window_np = (double*)calloc(npoints/6,sizeof(double));
						Result_np = (double*)calloc(npoints/6,sizeof(double));
						LinEv1D(vectLocation_1,npoints/6,0,1,Window_np);	   
							
						switch (winType)
						{
							case 0:
								for(int i=0; i < npoints/6 - 1; ++i)
									Window_np[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints/6)) + a2 * cos((4.0 * PI * i)/(double)(npoints/6));
								
								break;
							case 1:
								TriWin(Window_np, npoints/6);
								break;
						}
						Mul1D(vectLocation_1,Window_np,npoints/6,Result_np);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG, Result_np, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						realloc(Window_np, 0);
						realloc(Result_np, 0);
						
						//2
						Cheb1 = (double*)calloc(npoints/6,sizeof(double));
						Window_np = (double*)calloc(npoints/6,sizeof(double));
						vectLocation_2 = rawWin + (int)(FStart * npoints/6);
						
						GetCtrlVal(panel, FREQ_PANEL_ORDER, &Order);
						GetCtrlVal(panel, FREQ_PANEL_RIPPLE, &Ripple);
						Ch_BPF(vectLocation_2, npoints/6, sampleRate,  sampleRate/6, sampleRate * 2/6, Ripple, Order, Cheb1);
						
						//InvCh_BPF (); //atenuarea de 40 db
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						
						LinEv1D(Cheb1,npoints/6,0,1,Window_np);	   
							
						switch (winType)
						{
							case 0:
								for(int i=0; i < npoints/6 - 1; ++i)
									Window_np[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints/6)) + a2 * cos((4.0 * PI * i)/(double)(npoints/6));
								break;
							case 1:
								TriWin(Window_np, npoints/6);
								break;
						}
						Mul1D(Cheb1,Window_np,npoints/6,Result_np1);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13, Result_np1, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						realloc(Cheb1,0);
						realloc(Window_np, 0);
						
						//3
						Cheb2 = (double*)calloc(npoints/6,sizeof(double));
						Window_np = (double*)calloc(npoints/6,sizeof(double));
						vectLocation_3 = rawWin + (int)(FStart * npoints/6);
						
						GetCtrlVal(panel, FREQ_PANEL_ORDER, &Order);
						GetCtrlVal(panel, FREQ_PANEL_BAND_ATTEND, &StopAttend);
						InvCh_BPF(vectLocation_3, npoints/6, sampleRate, sampleRate/6, sampleRate * 2/6, StopAttend, Order, Cheb2);
						//InvCh_BPF (); //atenuarea de 40 db
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						
						LinEv1D(Cheb2,npoints/6,0,1,Window_np);	   
							
						switch (winType)
						{
							case 0:
								for(int i=0; i < npoints/6 - 1; ++i)
									Window_np[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints/6)) + a2 * cos((4.0 * PI * i)/(double)(npoints/6));
								break;
							case 1:
								TriWin(Window_np, npoints/6);
								break;
						}
						Mul1D(Cheb2,Window_np,npoints/6,Result_np2);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23, Result_np2, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						realloc(Cheb2,0);
						realloc(Window_np, 0);
						break;
					case 2: //Option "Windowing on raw signal"
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 0);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 0);
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						
						vectLocation_1 = rawWin + (int)(FStart * npoints/6);
						Window_np = (double*)calloc(npoints/6,sizeof(double));
						Result_np = (double*)calloc(npoints/6,sizeof(double));
						LinEv1D(vectLocation_1,npoints/6,0,1,Window_np);	   
							
						switch (winType)
						{
							case 0:
								for(int i=0; i < npoints/6 - 1; ++i)
									Window_np[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints/6)) + a2 * cos((4.0 * PI * i)/(double)(npoints/6));
								break;
							case 1:
								TriWin(Window_np, npoints/6);
								break;
						}
						Mul1D(vectLocation_1,Window_np,npoints/6,Result_np);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG, Result_np, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						realloc(Window_np, 0);
						realloc(Result_np, 0);
						break;
					case 3: //Option "Windowing Cheb1"
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 0);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 0);
						
						Cheb1 = (double*)calloc(npoints/6,sizeof(double));
						Window_np = (double*)calloc(npoints/6,sizeof(double));
						vectLocation_2 = rawWin + (int)(FStart * npoints/6);
						
						GetCtrlVal(panel, FREQ_PANEL_ORDER, &Order);
						GetCtrlVal(panel, FREQ_PANEL_RIPPLE, &Ripple);
						Ch_BPF(vectLocation_2, npoints/6, sampleRate,  sampleRate/6, sampleRate * 2/6, Ripple, Order, Cheb1);
						//InvCh_BPF (); //atenuarea de 40 db
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						
						LinEv1D(Cheb1,npoints/6,0,1,Window_np);	   
							
						switch (winType)
						{
							case 0:
								for(int i=0; i < npoints/6 - 1; ++i)
									Window_np[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints/6)) + a2 * cos((4.0 * PI * i)/(double)(npoints/6));
								break;
							case 1:
								TriWin(Window_np, npoints/6);
								break;
						}
						Mul1D(Cheb1,Window_np,npoints/6,Result_np1);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13, Result_np1, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						realloc(Cheb1,0);
						realloc(Window_np, 0);
						break;
					case 4: //Option "Windowing Cheb2"
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 0);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 0);
						
						Cheb2 = (double*)calloc(npoints/6,sizeof(double));
						Window_np = (double*)calloc(npoints/6,sizeof(double));
						vectLocation_3 = rawWin + (int)(FStart * npoints/6);
						
						GetCtrlVal(panel, FREQ_PANEL_ORDER, &Order);
						GetCtrlVal(panel, FREQ_PANEL_BAND_ATTEND, &StopAttend);
						InvCh_BPF(vectLocation_3, npoints/6, sampleRate, sampleRate/6, sampleRate * 2/6, StopAttend, Order, Cheb2);
						//InvCh_BPF (); //atenuarea de 40 db
						
						GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
						
						LinEv1D(Cheb2,npoints/6,0,1,Window_np);	   
							
						switch (winType)
						{
							case 0:
								for(int i=0; i < npoints/6 - 1; ++i)
									Window_np[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(npoints/6)) + a2 * cos((4.0 * PI * i)/(double)(npoints/6));
								break;
							case 1:
								TriWin(Window_np, npoints/6);
								break;
						}
						Mul1D(Cheb2,Window_np,npoints/6,Result_np2);
						
						DeleteGraphPlot (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23, -1, VAL_IMMEDIATE_DRAW);
						PlotY (frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23, Result_np2, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						realloc(Cheb2,0);
						realloc(Window_np, 0);
						break;
					case 5:
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 1);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 1);
						
						//luam de pe interfata numarul de puncte pentru care se calculeaza spectrul
						GetCtrlVal(panel, FREQ_PANEL_NUMBER_POINTS, &NumberOfPoints);
						
						//afisam pe interfata numarul de ferestre necesare reprezentarii intregului spectru
						SetCtrlVal(panel, FREQ_PANEL_NUMBER_OF_WINDOWS, ((npoints/6)/NumberOfPoints)+1);
						
						//alocare memorie pentru numarul de puncte
						autoSpectrum=(double*)calloc(NumberOfPoints/2,sizeof(double));//spectrul de putere
						convertedSpectrum=(double*)calloc(NumberOfPoints/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului
						
						double *Seconds;
						Seconds = (double*)calloc(NumberOfPoints,sizeof(double));
						
						int timer = 0;
						while(timer < npoints/6)
						{
							if(timer + NumberOfPoints <= npoints/6)	
							{
								for(int i=0; i < NumberOfPoints; ++i)
								{
									Seconds[i] = vectLocation_1[i+timer];//copiez in Seconds o fereastra de puncte din temp
								}
							}
							else
							{
								for(int i=0; i < npoints/6-timer; ++i)
								{
									Seconds[i] = vectLocation_1[i+timer];
								}
								for(int i=npoints/6-timer; i<NumberOfPoints; ++i)
								{
									Seconds[i]=0;
								}
							}
							
							char unit[32]="V";
							
							window_timer = (double*)calloc(NumberOfPoints,sizeof(double));
							result_timer = (double*)calloc(NumberOfPoints,sizeof(double));
							
							GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
							LinEv1D(Seconds,NumberOfPoints,0,1,window_timer);	   
								
							switch (winType)
							{
								case 0:
									//BlkHarrisWin(window_timer,NumberOfPoints);
									for(int i=0; i < NumberOfPoints - 1; ++i)
										window_timer[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(NumberOfPoints)) + a2 * cos((4.0 * PI * i)/(double)(NumberOfPoints));
									break;
								case 1:
									TriWin(window_timer, NumberOfPoints);
									break;
							}
							Mul1D(Seconds,window_timer,NumberOfPoints,result_timer);
						
							/*
							switch (winType)
							{
								case 0:
									ScaledWindowEx (result_timer,NumberOfPoints, BLKMAN, 0, &winConst);
									break;
								case 1:
									ScaledWindowEx (result_timer,NumberOfPoints, TRIANGLE, 0, &winConst);
									break;
							}*/
							//se calculeaza partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
							AutoPowerSpectrum(result_timer, NumberOfPoints, 1.0/sampleRate, autoSpectrum, &df);
							//functia returneaza frequencyPeak si powerPeak
							PowerFrequencyEstimate (autoSpectrum, NumberOfPoints/2, -1.0, winConst, df, 7, &freqPeak,&powerPeak);
							//se converteste spectrul de intrare în formate ce permit o reprezentare grafica mai convenabila
							SpectrumUnitConversion(autoSpectrum, NumberOfPoints/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
							//afisam spectrul pe grafic
							PlotWaveform(panel,  FREQ_PANEL_FREQUENCY_RAW_SIG, convertedSpectrum, NumberOfPoints /2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_RED);		
							//afisam pe interfata valorile pentru frequencyPeak si powerPeak
							SetCtrlVal(panel, FREQ_PANEL_FREQUENCY_PEAK, freqPeak);
							SetCtrlVal(panel, FREQ_PANEL_POWER_PEAK, powerPeak);
							//afisam pe interfata numarul ferestrei curente
							SetCtrlVal(panel, FREQ_PANEL_CURRENT_WINDOW, (timer/NumberOfPoints)+1);
							
							timer += NumberOfPoints;
							
							Delay(1);
							DeleteGraphPlot (panel, FREQ_PANEL_FREQUENCY_RAW_SIG, -1, VAL_IMMEDIATE_DRAW);
							realloc(window_timer, 0);
							realloc(result_timer, 0);
						}
						realloc(Seconds, 0);
						realloc(convertedSpectrum, 0);
						realloc(autoSpectrum, 0);
						break;
					case 6:
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 1);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 1);
						
						//luam de pe interfata numarul de puncte pentru care se calculeaza spectrul
						GetCtrlVal(panel, FREQ_PANEL_NUMBER_POINTS, &NumberOfPoints);
						
						//afisam pe interfata numarul de ferestre necesare reprezentarii intregului spectru
						SetCtrlVal(panel, FREQ_PANEL_NUMBER_OF_WINDOWS, ((npoints/6)/NumberOfPoints)+1);
						
						//alocare memorie pentru numarul de puncte
						autoSpectrum=(double*)calloc(NumberOfPoints/2,sizeof(double));//spectrul de putere
						convertedSpectrum=(double*)calloc(NumberOfPoints/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului
						
						double *Seconds_CB1;
						Seconds_CB1 = (double*)calloc(NumberOfPoints,sizeof(double));
						
						int timer_CB1 = 0;
						while(timer_CB1 < npoints/6)
						{
							if(timer_CB1 + NumberOfPoints <= npoints/6)	
							{
								for(int i=0; i < NumberOfPoints; ++i)
								{
									Seconds_CB1[i] = Result_np1[i+timer_CB1];//copiez in Seconds o fereastra de puncte din temp
								}
							}
							else
							{
								for(int i=0; i < npoints/6-timer_CB1; ++i)
								{
									Seconds_CB1[i] = Result_np1[i+timer_CB1];
								}
								for(int i=npoints/6-timer_CB1; i<NumberOfPoints; ++i)
								{
									Seconds_CB1[i]=0;
								}
							}
							
							char unit[32]="V";
							
							window_timer = (double*)calloc(NumberOfPoints,sizeof(double));
							result_timer = (double*)calloc(NumberOfPoints,sizeof(double));
							
							GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
							LinEv1D(Seconds_CB1,NumberOfPoints,0,1,window_timer);	   
								
							switch (winType)
							{
								case 0:
									//BlkHarrisWin(window_timer,NumberOfPoints);
									for(int i=0; i < NumberOfPoints - 1; ++i)
										window_timer[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(NumberOfPoints)) + a2 * cos((4.0 * PI * i)/(double)(NumberOfPoints));
									break;
								case 1:
									TriWin(window_timer, NumberOfPoints);
									break;
							}
							Mul1D(Seconds_CB1,window_timer,NumberOfPoints,result_timer);
						
							/*
							switch (winType)
							{
								case 0:
									ScaledWindowEx (result_timer,NumberOfPoints, BLKMAN, 0, &winConst);
									break;
								case 1:
									ScaledWindowEx (result_timer,NumberOfPoints, TRIANGLE, 0, &winConst);
									break;
							}*/
							//se calculeaza partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
							AutoPowerSpectrum(result_timer, NumberOfPoints, 1.0/sampleRate, autoSpectrum, &df);
							//functia returneaza frequencyPeak si powerPeak
							PowerFrequencyEstimate (autoSpectrum, NumberOfPoints/2, -1.0, winConst, df, 7, &freqPeak,&powerPeak);
							//se converteste spectrul de intrare în formate ce permit o reprezentare grafica mai convenabila
							SpectrumUnitConversion(autoSpectrum, NumberOfPoints/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
							//afisam spectrul pe grafic
							PlotWaveform(panel,  FREQ_PANEL_FREQUENCY_CHEB_13, convertedSpectrum, NumberOfPoints /2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_RED);		
							//afisam pe interfata valorile pentru frequencyPeak si powerPeak
							SetCtrlVal(panel, FREQ_PANEL_FREQUENCY_PEAK, freqPeak);
							SetCtrlVal(panel, FREQ_PANEL_POWER_PEAK, powerPeak);
							//afisam pe interfata numarul ferestrei curente
							SetCtrlVal(panel, FREQ_PANEL_CURRENT_WINDOW, (timer_CB1/NumberOfPoints)+1);
							
							timer_CB1 += NumberOfPoints;
							
							Delay(1);
							DeleteGraphPlot (panel, FREQ_PANEL_FREQUENCY_CHEB_13, -1, VAL_IMMEDIATE_DRAW);
							realloc(window_timer, 0);
							realloc(result_timer, 0);
							
						}
						realloc(Seconds_CB1, 0);
						realloc(convertedSpectrum, 0);
						realloc(autoSpectrum, 0);
						break;
					case 7:
						SetCtrlAttribute(panel, FREQ_PANEL_PREV, ATTR_DIMMED, 1);
						SetCtrlAttribute(panel, FREQ_PANEL_NEXT, ATTR_DIMMED, 1);
						
						//luam de pe interfata numarul de puncte pentru care se calculeaza spectrul
						GetCtrlVal(panel, FREQ_PANEL_NUMBER_POINTS, &NumberOfPoints);
						
						//afisam pe interfata numarul de ferestre necesare reprezentarii intregului spectru
						SetCtrlVal(panel, FREQ_PANEL_NUMBER_OF_WINDOWS, ((npoints/6)/NumberOfPoints)+1);
						
						//alocare memorie pentru numarul de puncte
						autoSpectrum=(double*)calloc(NumberOfPoints/2,sizeof(double));//spectrul de putere
						convertedSpectrum=(double*)calloc(NumberOfPoints/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului
						
						double *Seconds_CB2;
						Seconds_CB2 = (double*)calloc(NumberOfPoints,sizeof(double));
						
						int timer_CB2 = 0;
						while(timer_CB2 < npoints/6)
						{
							if(timer_CB2 + NumberOfPoints <= npoints/6)	
							{
								for(int i=0; i < NumberOfPoints; ++i)
								{
									Seconds_CB2[i] = Result_np2[i+timer_CB2];//copiez in Seconds o fereastra de puncte din temp
								}
							}
							else
							{
								for(int i=0; i < npoints/6-timer_CB2; ++i)
								{
									Seconds_CB2[i] = Result_np2[i+timer_CB2];
								}
								for(int i=npoints/6-timer_CB2; i<NumberOfPoints; ++i)
								{
									Seconds_CB2[i]=0;
								}
							}
							
							char unit[32]="V";
							
							window_timer = (double*)calloc(NumberOfPoints,sizeof(double));
							result_timer = (double*)calloc(NumberOfPoints,sizeof(double));
							
							GetCtrlVal(frequencyPanel,FREQ_PANEL_WINDOW_TYPE,&winType);
							LinEv1D(Seconds_CB2,NumberOfPoints,0,1,window_timer);	   
								
							switch (winType)
							{
								case 0:
									//BlkHarrisWin(window_timer,NumberOfPoints);
									for(int i=0; i < NumberOfPoints - 1; ++i)
										window_timer[i] = a0 - a1 * cos((2.0 * PI * i)/(double)(NumberOfPoints)) + a2 * cos((4.0 * PI * i)/(double)(NumberOfPoints));
									break;
								case 1:
									TriWin(window_timer, NumberOfPoints);
									break;
							}
							Mul1D(Seconds_CB2,window_timer,NumberOfPoints,result_timer);
							/*
							switch (winType)
							{
								case 0:
									ScaledWindowEx (result_timer,NumberOfPoints, BLKMAN, 0, &winConst);
									break;
								case 1:
									ScaledWindowEx (result_timer,NumberOfPoints, TRIANGLE, 0, &winConst);
									break;
							}*/
							//se calculeaza partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
							AutoPowerSpectrum(result_timer, NumberOfPoints, 1.0/sampleRate, autoSpectrum, &df);
							//functia returneaza frequencyPeak si powerPeak
							PowerFrequencyEstimate (autoSpectrum, NumberOfPoints/2, -1.0, winConst, df, 7, &freqPeak,&powerPeak);
							//se converteste spectrul de intrare în formate ce permit o reprezentare grafica mai convenabila
							SpectrumUnitConversion(autoSpectrum, NumberOfPoints/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
							//afisam spectrul pe grafic
							PlotWaveform(panel,  FREQ_PANEL_FREQUENCY_CHEB_23, convertedSpectrum, NumberOfPoints /2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_RED);		
							//afisam pe interfata valorile pentru frequencyPeak si powerPeak
							SetCtrlVal(panel, FREQ_PANEL_FREQUENCY_PEAK, freqPeak);
							SetCtrlVal(panel, FREQ_PANEL_POWER_PEAK, powerPeak);
							//afisam pe interfata numarul ferestrei curente
							SetCtrlVal(panel, FREQ_PANEL_CURRENT_WINDOW, (timer_CB2/NumberOfPoints)+1);
							
							timer_CB2 += NumberOfPoints;
							
							Delay(1);
							DeleteGraphPlot (panel, FREQ_PANEL_FREQUENCY_CHEB_23, -1, VAL_IMMEDIATE_DRAW);
							realloc(window_timer, 0);
							realloc(result_timer, 0);
							
						}
						realloc(Seconds_CB2, 0);
						realloc(convertedSpectrum, 0);
						realloc(autoSpectrum, 0);
						break;
				}
			break;
	}
	return 0;
}

int CVICALLBACK OnSave (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	int imgHandle;  
	char fileName[256] = {0}, fileName1[256]={0}, fileName2[256]={0}, fileName3[256]={0}, fileName4[256]={0};
	switch (event)
	{
		case EVENT_COMMIT:
			//luam data si ora
			GetSystemDate(&month, &day, &year);
			GetSystemTime(&hour, &minute, &second);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName, "ROW_DATA_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(panel,WAVE_PANEL_GRAPH,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName,JPEG_PROGRESSIVE,100);
			
			
			//setam numele cu care salvam imaginea
			sprintf(fileName1, "FILTERED_DATA__%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imagine
			GetCtrlDisplayBitmap(panel,WAVE_PANEL_GRAPH_2,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName1,JPEG_PROGRESSIVE,100);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName2, "WINDOWING_RAW_SIGNAL_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(frequencyPanel,FREQ_PANEL_WINDOW_RAW_SIG,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName2,JPEG_PROGRESSIVE,100);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName3, "CHEBYSHEV1_3_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(frequencyPanel,FREQ_PANEL_WINDOW_CHEB_13,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName3,JPEG_PROGRESSIVE,100);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName4, "CHEBYSHEV2_3_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(frequencyPanel,FREQ_PANEL_WINDOW_CHEB_23,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName4,JPEG_PROGRESSIVE,100);
			
		
			DiscardBitmap(imgHandle);
			
			break;
	}
	return 0;
}
