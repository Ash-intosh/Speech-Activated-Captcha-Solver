#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <string>
// For creating directories
#pragma comment(lib, "winmm.lib")
using namespace std;

#define LENGTH_WAV 16025 * 2

short int waveIn[LENGTH_WAV];  // Array where the sound sample will be stored
double data_test[50000] = {0};
int countt = 0;
int cnt = LENGTH_WAV;
#define MAX_FRAME 100
#define FRAME_SIZE 320
#define P 12 // Size of the vector, consists of the 12 CI for a frame
#define K 32 // Codebook size
#define N 5
#define M 32
#define T 100
#define MIN 1e-15
#define PIE 3.142857142857
#define D 10 // Number of words

// Declaring all the Matrices
double Alpha_test[T][N], A_test[N][N], B_test[N][M], A_test_2[D][N][N], B_test_2[D][N][M], Pie_test[N];
int O_test[T], O_test_all[100][T];

int count_tst = LENGTH_WAV;
double Codebook_testing[K][P];
int Observation_testing[MAX_FRAME];
double tokhura_weights_testing[P] = {1, 3, 7, 13, 19, 22, 25, 33, 42, 50, 56, 61};

double R_test[MAX_FRAME][P + 1]; // Array for storing auto correlation matrix
double Ai_test[MAX_FRAME][P + 1]; // Array for storing Levinson Durbin Coefficient
double C_test[MAX_FRAME][P + 1]; // Array for storing Cepstral Coefficients
double Cri_test[MAX_FRAME][P + 1]; // Array for storing Raised Cosine Window values

double all_frames_test[MAX_FRAME][FRAME_SIZE];

void initialize_Global_Arrays_test(){
	for(int f = 0; f < MAX_FRAME; f++){
		for(int p = 0; p < P + 1; p++){
			R_test[f][p] = 0; 
			Ai_test[f][p] = 0;
			C_test[f][p] = 0;
			Cri_test[f][p] = 0;
		}
	}
}

// Calculating DC Offset
double calculateDCOffset_test(){
	double sum = 0.0;
	for(int i = 0; i < count_tst; i++){
		sum += data_test[i];
	}
	return sum/count_tst;
}

void correctDCShift_test() {
    double dcOffset = calculateDCOffset_test();
	if(dcOffset == 0) return;
    for (int i = 0; i < count_tst; i++) {
       data_test[i] = (data_test[i] - dcOffset);
    }
}

void normalizeit_test() {
    // Step 1: Find the current min and max values
	double minn = data_test[0];
	double maxx = data_test[0];

    for (int i = 1; i < count_tst; i++) {
        if (data_test[i] < minn) {
            minn = data_test[i];
        }
		if (data_test[i] > maxx) {
            maxx = data_test[i];
        }
    }

    // Step 2: Normalize the array to the range [-5000.0, 5000.0]
    for (int i = 0; i < count_tst; i++) {
		data_test[i] = -5000 + (((data_test[i] - (minn)) / (maxx - minn)) * 10000);
    }
}

void steady_test() {
	// Considering all the frames
    for (int i = 0; i < MAX_FRAME; i++) {
        for (int j = 0; j < FRAME_SIZE; j++) {
            all_frames_test[i][j] = data_test[(i)*320 + j];
        }
    }
}

double window_test[320];

void hammingWindow_test() {
    for(int i = 0; i < 320; i++){
		window_test[i] = 0.54 - (0.46*cos(2*(PIE*i) / (320 - 1)));
	}
}

void impHamming_test(){
	for(int f = 0; f < MAX_FRAME; f++){
		for(int j = 0; j < 320; j++){
			all_frames_test[f][j] *= window_test[j];
		}
	}
}

// int i is the index of the frame to process.
void calculateRI_test(int i){
	//calculating RI with the help of Levinson Durbin Algo
	//Here the p = 12 (order) and a lag k is created here after every loop
	for(int k = 0; k <= P; k++){
		R_test[i][k] = 0.0;
		for(int j = 0; j < FRAME_SIZE - k; j++){
			R_test[i][k] += (all_frames_test[i][j] * all_frames_test[i][j + k]);
		}		
	}
}

// int i is the index of the frame to process.
void calculateAI_test(int f){
	//Calculating Ai with the help of levinson durbin algo
	double e[P + 1] = {0.0}, k[P + 1] = {0.0};
	double x[P + 1][P + 1] = {0.0};

	e[0] = R_test[f][0];

	for(int i = 1; i <= P; i++){
		double val = 0.0;

		for(int j = 1; j<= i-1; j++){
			val += x[j][i-1] * R_test[f][i-j];
		}

		k[i] = (R_test[f][i] - val) / e[i-1];

		x[i][i] = k[i];
			for(int j = 1; j <= i-1; j++){
				x[j][i] = x[j][i-1] - (k[i] * x[i-j][i-1]);
			}

		e[i] =(1 - (k[i] * k[i]))* e[i-1];
	}

	for(int i = 1; i<= P; i++){
		Ai_test[f][i] = x[i][P];
	}
}

double tapered_test[12];

void tapered_window_test(){
	for (int m = 1; m <= 12; ++m) {
       tapered_test[m] = (1 + ((12/2) * sin((PIE * m) / 12)));
    }
}

void calculateCI_test(int f){
	tapered_window_test();
    // Initialize the zeroth cepstral coefficient C0
    C_test[f][0] = 0.0;
    Cri_test[f][0] = 0.0;

	for (int i = 1; i <= P; i++) {

        C_test[f][i] = Ai_test[f][i];
		double sum = 0.0;

		for (int k = 1; k < i; k++) { 
			if (i - k >= 0){
				sum += k * C_test[f][k] * Ai_test[f][i - k];
			}
		}

		C_test[f][i] += sum / i;

		// Raised Sine Window
		Cri_test[f][i] += C_test[f][i] * (1 + (12/2) * sin(PIE * i / 12));
	}
}

void get_Codebook_from_CSV_test(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening Codebook file!\n");
        exit(1);
    }

    while(!feof(file)) {
    for(int k = 0; k < K; k++){   
        for (int j = 0; j < P; j++) {
            fscanf(file, "%lf", &Codebook_testing[k][j]);
        }
    }
    }

    fclose(file);
}

double tokhura_distance_test(double v1[P], double v2[P]) {
    double tokhuradist = 0;
    for (int i = 0; i < P; i++) {
        tokhuradist += tokhura_weights_testing[i] * (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return tokhuradist;
}

// Function to write data to a file
void PutDatainanArray_test(char* filename) {

	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		perror("Error opening to put Ci values to a file");
		exit(1);
	}

    // Write data to the file
    for (int i = 0; i < MAX_FRAME; i++) {
        for (int j = 1; j <= P; j++) {
            fprintf(file, "%lf ", Cri_test[i][j]);
            Cri_test[i][j] = 0;
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void get_Cri_test(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening Codebook file!\n");
        exit(1);
    }

    while(!feof(file)) {
        for(int k = 0; k < MAX_FRAME; k++){   
            for (int j = 0; j < P; j++) {
                fscanf(file, "%lf", &Cri_test[k][j]);
            }
        }
    }

    fclose(file);
}

void GetDatainanArray(char* filename){
    FILE *file = fopen(filename, "r");
	if (file == NULL){
		printf("Couldn't open file, exiting....\n");
		exit(1);
	}  

	while(!feof(file)){
		if (fscanf(file, "%lf", &data_test[countt]) == 1 && countt < 50000){
			countt++;
		}
		else{
			break;
		}
	}
    fclose(file);
}

void Get_Observations_test() {
    for (int i = 0; i < MAX_FRAME; i++) {
        double min_dist = DBL_MAX;
        int min_id = -1;
        for (int j = 0; j < 32; j++) {
            double dist = tokhura_distance_test(Cri_test[i], Codebook_testing[j]);
            if (dist < min_dist) {
                min_dist = dist;
                min_id = j;
            }
        }
        O_test[i] = min_id + 1;  // Assign vector to the nearest region
    }
}

double forward_procedure_test_forall(int itr, int model){

    // Step 1: Initialization
    for(int i = 0; i < N; i++){
        Alpha_test[0][i] = (Pie_test[i] * B_test_2[model][i][O_test[0]-1]);
    }

    // Step 2: Induction
    for(int t = 1; t < T; t++){
        for(int j = 0; j < N; j++){
            Alpha_test[t][j] = 0;
            for(int s = 0; s < N; s++){
                Alpha_test[t][j] += (Alpha_test[t-1][s] * A_test_2[model][s][j]);
            }
            Alpha_test[t][j] *= B_test_2[model][j][O_test[t]-1];
        }
    }

    // Step 3: Termination
    double result = 0;
    for(int s = 0; s < N; s++){
        result += Alpha_test[T-1][s];
    }

    //printf("Final result: %e\n", result);
    return result;
}

void getting_avg_model_values_test_forall(int itr){

    char A_filename[100] = "";
    char B_filename[100] = "";
    char PI_filename[100] = "Initial_Model/PI_matrix.txt";
    for(int m = 0; m < 10; m++){
        //sprintf(A_filename, "AVG_Model_HMM/itr_%d/244101007_Model_%d/A.txt", itr, m);
        //sprintf(B_filename, "AVG_Model_HMM/itr_%d/244101007_Model_%d/B.txt", itr, m);

        // Inputing files 
        FILE* A_file = fopen(A_filename, "r");
        if(A_file == NULL)
        {
            printf("\n%d %d Cannot open file\n",itr,m);
            system("pause");
            exit(1);
        }
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                double tempa = 0;
                fscanf(A_file, "%lf ", &tempa);
                A_test_2[m][i][j] = tempa;
            }
        }
    
        fclose(A_file);

        FILE* B_file = fopen(B_filename, "r");
        if(B_file == NULL){
            printf("Unable to open the file\n");
        }

        for(int i = 0; i < N; i++){
            for(int j = 0; j < M; j++){
                double temp = 0;
                fscanf(A_file, "%lf ", &temp);
                B_test_2[m][i][j] = temp;
            }
        }
        
        fclose(B_file);
    }
    

    FILE* PI_file = fopen(PI_filename, "r");
	if(PI_file == NULL){
		printf("\nCannot open PI file file\n");
        system("pause");
        exit(1);
        }

    for(int i = 0; i < N; i++){
        fscanf(PI_file, "%lf ", &Pie_test[i]);
    }

    fclose(PI_file);

    for(int i = 0; i < T; i++){
        for(int j = 0; j < N; j++){
            Alpha_test[i][j] = 0;
        }
    }
}


int testing(){
    char filename[100] = "";
	char outputfilename[100] = "";
    char CIfilename[100] = "";
    char OSeqfilename[100] = "";
    char ModelAfilename[100] = "";
    char ModelBfilename[100] = "";
    char dirName[100] = "";
    char highest_pstar_file[100] = "";

    // To train on ZERO

	bool yes = true;
	int itr = 2;
    int pred_digit_is = -1;
    getting_avg_model_values_test_forall(itr);
	do{

        double highest_pstar = 0;
        initialize_Global_Arrays_test();

        //GetDatainanArray("244101007_0_1.txt");
        ifstream infile("audio_data.txt");
        string line;
        int result = 0;
    
        // Simulate a process for extracting a numeric answer from the audio data
        // (For simplicity, we're just reading a sum of numbers as an example)
        int i = 0;
        while (getline(infile, line)) {
            data_test[i] = stoi(line);
            i++;
        }

        correctDCShift_test();
        normalizeit_test();
        steady_test();
        hammingWindow_test();
        impHamming_test();

        for (int f = 0; f < MAX_FRAME; f++) {
            calculateRI_test(f);          // Compute autocorrelation for the frame
            calculateAI_test(f);           // Compute LPC coefficients using Levinson-Durbin
            calculateCI_test(f); // Compute cepstral coefficients from LPC coefficients
        }
		
        //sprintf(CIfilename, "CI_temp/244101007_CI.txt");
        PutDatainanArray_test(CIfilename);	// Write the processed data to the output file
                
        get_Codebook_from_CSV_test("Codebook/Codebook.txt");
        get_Cri_test(CIfilename);

        Get_Observations_test();

        double best_result = 0;
        for(int model = 0; model < 10; model++){
			double result = forward_procedure_test_forall(itr, model);
                //printf("\nProbability with Model for digit %d is %e", model, result);
                if(result > best_result){
                    best_result = result;
                    pred_digit_is = model;
                }
        }
        //printf("\n Final Prediction is %d", pred_digit_is);

    }while(false);
            // printf("\nCorrect pred is %lf ", correct_pred);
            // printf("\nInCorrect pred is %lf", incorrect_pred);
            // double Accuracy = (correct_pred / (incorrect_pred + correct_pred)) * 100;
            // printf("\nAccuracy is %lf", Accuracy);
    return pred_digit_is;   

}

int main()
{
	int answer = testing();
    std::cout << answer << "\n" << std::endl;
	return 0;
}