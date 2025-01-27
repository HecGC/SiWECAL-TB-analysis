#include "siwecaldecoded.h"
#define PBSTR "||"
#define PBWIDTH 0

//#include <bits/stdc++.h>

int SimpleCoincidenceTagger(int ilayer, int ichip, int maxnhit=5, int bcid_ref=0){
  
  int bcid_seen=0;
  int bcid_seen_slb[30]={0};
  for(int islboard=0; islboard<n_slboards; islboard++) {
    for(int isca=0; isca<15; isca++) {
      if( bcid[islboard][ichip][isca]<50 || nhits[islboard][ichip][isca]>maxnhit || ( bcid[islboard][ichip][isca]<1000 && bcid[islboard][ichip][isca]>900) )
        continue;
      if(islboard==ilayer) continue;
      if( fabs(bcid[islboard][ichip][isca]-bcid_ref)<2)
        bcid_seen_slb[islboard]++;
    }//end isca                                                                                      

    if(bcid_seen_slb[islboard]>0) bcid_seen++;
  }

  return bcid_seen;

}

double weigthedAv(double val[], double err[], int n) {

  double av=0;
  double errsum=0;
  for(int i = 0; i < n; i++)    {
    if(val[i]>0 ) {
      av+= val[i]*(1./err[i]);
      errsum+=(1./err[i]);
    }
  }
  
  if (errsum>0) return av/=errsum;
  return 0.;
}


int mostFrequent(int arr[], int n)
{
    // Sort the array
    sort(arr, arr + n);
 
    // find the max frequency using linear traversal
    int max_count = 1, res = arr[0], curr_count = 1;
    for (int i = 1; i < n; i++) {
        if (arr[i] == arr[i - 1])
            curr_count++;
        else {
            if (curr_count > max_count) {
                max_count = curr_count;
                res = arr[i - 1];
            }
            curr_count = 1;
        }
    }
 
    // If last element is most frequent
    if (curr_count > max_count)
    {
        max_count = curr_count;
        res = arr[n - 1];
    }
 
    return res;
}

void printProgress(double percentage) {
    int val = (int) (percentage );
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "-");
    fflush(stdout);
}


void ReadMasked(TString filename) 
{

  std::ifstream reading_file(filename);
  if(!reading_file){
    cout<<" dameyo - damedame"<<endl;
  }
  for(int i=0; i<16; i++) {
    for(int j=0; j<64; j++) {
      masked[i][j] = 0;
    }
  }

  Int_t tmp_chip = 0,tmp_channel = 0;
  Int_t tmp_masked = 0;
  TString tmpst;
  reading_file >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst ;
  reading_file >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst  ;

  cout<<"Read Masked: "<<filename<<endl;
  while(reading_file){
    reading_file >> tmp_chip >> tmp_channel >> tmp_masked ;
    masked[tmp_chip][tmp_channel] = tmp_masked;
  }

  double nmasked=0.;
  for(int i=0; i<16; i++) {
    for(int j=0; j<64; j++) {
      if(masked[i][j]==1) nmasked++;
    }
  }
  nmasked=100.*nmasked/1024;
  cout<< "In file " <<filename << " we read that "<<nmasked<<"% of channels are masked"<<endl;
  
}

void ReadMap(TString filename, int slboard) 
{

  std::ifstream reading_file(filename);
  if(!reading_file){
    cout<<" dameyo - damedame"<<endl;
  }
  for(int i=0; i<16; i++) {
    for(int j=0; j<64; j++) {
      map_pointX[slboard][i][j] = -1000.;
      map_pointY[slboard][i][j] = -1000.;
    }
  }

  Int_t tmp_chip = 0,tmp_channel = 0;
  Float_t tmp_x0 = 0 ,tmp_y0 = 0 , tmp_x = 0 , tmp_y = 0 ;
  TString tmpst;
  reading_file >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst ;
  while(reading_file){
    reading_file >> tmp_chip >> tmp_x0 >> tmp_y0 >> tmp_channel >> tmp_x >> tmp_y ;
    map_pointX[slboard][tmp_chip][tmp_channel] = -tmp_x ;
    map_pointY[slboard][tmp_chip][tmp_channel] = -tmp_y ;
  }

}


// LANDAU STUFF
//---------------------------------------------------------------------------------------

Double_t langaufun(Double_t *x, Double_t *par) {
  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation), 
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.
  
  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location
  
  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
  
  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;
  
  
  // MP shift correction
  mpc = par[1] - mpshift * par[0]; 
  
  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];
  
  step = (xupp-xlow) / np;
  
  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0],xx,par[3]);
    
    xx = xupp - (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0],xx,par[3]);
  }
  
  return (par[2] * step * sum * invsq2pi / par[3]);
}



TF1* langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF)
{
  // Once again, here are the Landau * Gaussian parameters:
  //   par[0]=Width (scale) parameter of Landau density
  //   par[1]=Most Probable (MP, location) parameter of Landau density
  //   par[2]=Total area (integral -inf to inf, normalization constant)
  //   par[3]=Width (sigma) of convoluted Gaussian function
  //
  // Variables for langaufit call:
  //   his             histogram to fit
  //   fitrange[2]     lo and hi boundaries of fit range
  //   startvalues[4]  reasonable start values for the fit
  //   parlimitslo[4]  lower parameter limits
  //   parlimitshi[4]  upper parameter limits
  //   fitparams[4]    returns the final fit parameters
  //   fiterrors[4]    returns the final fit errors
  //   ChiSqr          returns the chi square
  //   NDF             returns ndf

  Int_t i;
  Char_t FunName[100];

  sprintf(FunName,"Fitfcn_%s",his->GetName());

  TF1 *ffitold = (TF1*)gROOT->GetListOfFunctions()->FindObject(FunName);
  if (ffitold) delete ffitold;

  TF1 *ffit = new TF1(FunName,langaufun,fitrange[0],fitrange[1],4);
  ffit->SetParameters(startvalues);
  ffit->SetParNames("Width","MP","Area","GSigma");
   
  for (i=0; i<4; i++) {
    ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
  }

  his->Fit(FunName,"RBQM");   // fit within specified range, use ParLimits, quiet, improve fit results (TMINUIT)

  ffit->GetParameters(fitparams);    // obtain fit parameters
  for (i=0; i<4; i++) {
    fiterrors[i] = ffit->GetParError(i);     // obtain fit parameter errors
  }
  ChiSqr[0] = ffit->GetChisquare();  // obtain chi^2
  NDF[0] = ffit->GetNDF();           // obtain ndf

  return (ffit);              // return fit function

}



void ReadPedestalsProto(TString filename, bool invertedordering=false) 
{
  std::ifstream reading_file(filename);
  if(!reading_file){
    cout<<" ERROR  ----------------------------------- No pedestal file: "<<filename<<endl;
  } else {
    cout<<" Pedestal input file: "<<filename<<endl;
  }

  for(int islboard=0; islboard<15; islboard++) {
    std::vector<std::vector<std::vector<Double_t> > > chip_ped_mean_slb;
    std::vector<std::vector<std::vector<Double_t> > > chip_ped_error_slb;
    std::vector<std::vector<std::vector<Double_t> > > chip_ped_width_slb;

    //initialize pedestal vectors                                                                                                                                                                                                                                   
    for(int i=0; i<16; i++) {
      std::vector<std::vector<Double_t> > chip_ped_mean;
      std::vector<std::vector<Double_t> > chip_ped_error;
      std::vector<std::vector<Double_t> > chip_ped_width;

      for(int j=0; j<64; j++) {
	std::vector<Double_t> chn_ped_mean;
	std::vector<Double_t> chn_ped_error;
	std::vector<Double_t> chn_ped_width;

	for(int isca=0; isca<15; isca++) {
	  chn_ped_mean.push_back(-1);
	  chn_ped_error.push_back(-1);
	  chn_ped_width.push_back(-1);
	}
	chip_ped_mean.push_back(chn_ped_mean);
	chip_ped_error.push_back(chn_ped_error);
	chip_ped_width.push_back(chn_ped_width);
      }
      chip_ped_mean_slb.push_back(chip_ped_mean);
      chip_ped_error_slb.push_back(chip_ped_error);
      chip_ped_width_slb.push_back(chip_ped_width);
    }
    ped_mean_slboard.push_back(chip_ped_mean_slb);
    ped_error_slboard.push_back(chip_ped_error_slb);
    ped_width_slboard.push_back(chip_ped_width_slb);
  }

  Int_t tmp_layer=0, tmp_chip = 0,tmp_channel = 0;
  Double_t tmp_ped[15], tmp_error[15], tmp_width[15];
  for(int isca=0; isca<15; isca++) {
    tmp_ped[isca]=0.;
    tmp_error[isca]=0.;
    tmp_width[isca]=0.;
  }

  TString tmpst;
  reading_file >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >>  tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst  >> tmpst >> tmpst ;
  reading_file >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >>  tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst >> tmpst ;

  while(reading_file){
    reading_file >> tmp_layer >> tmp_chip >> tmp_channel >> tmp_ped[0] >> tmp_error[0] >> tmp_width[0] >> tmp_ped[1] >> tmp_error[1] >> tmp_width[1] >> tmp_ped[2] >> tmp_error[2] >> tmp_width[2] >> tmp_ped[3] >> tmp_error[3] >> tmp_width[3] >> tmp_ped[4] >> tmp_error[4] >> tmp_width[4] >> tmp_ped[5] >> tmp_error[5] >> tmp_width[5] >> tmp_ped[6] >> tmp_error[6] >> tmp_width[6] >> tmp_ped[7] >> tmp_error[7] >> tmp_width[7] >> tmp_ped[8] >> tmp_error[8] >> tmp_width[8] >> tmp_ped[9] >> tmp_error[9] >> tmp_width[9] >> tmp_ped[10] >> tmp_error[10] >> tmp_width[10] >> tmp_ped[11] >> tmp_error[11] >> tmp_width[11] >> tmp_ped[12] >> tmp_error[12] >> tmp_width[12] >> tmp_ped[13] >> tmp_error[13] >> tmp_width[13] >> tmp_ped[14] >> tmp_error[14] >> tmp_width[14];
    //    cout<<tmp_layer<<" "<<tmp_chip <<" "<< tmp_channel << " "<< tmp_ped[0]<<endl;                                                                                                                                                                                               
    for(int isca=0; isca<15; isca++) {
      if(tmp_ped[isca]>0. ){//&& (tmp_error[isca]<ped_error.at(tmp_chip).at(tmp_channel).at(isca) || ped_error.at(tmp_chip).at(tmp_channel).at(isca)==0) ){
	if(invertedordering==false) {
	  ped_mean_slboard.at(tmp_layer).at(tmp_chip).at(tmp_channel).at(isca)=tmp_ped[isca];
	  ped_error_slboard.at(tmp_layer).at(tmp_chip).at(tmp_channel).at(isca)=tmp_error[isca];
	  ped_width_slboard.at(tmp_layer).at(tmp_chip).at(tmp_channel).at(isca)=tmp_width[isca];
	} else {
	  ped_mean_slboard.at(14-tmp_layer).at(tmp_chip).at(tmp_channel).at(isca)=tmp_ped[isca];
	  ped_error_slboard.at(14-tmp_layer).at(tmp_chip).at(tmp_channel).at(isca)=tmp_error[isca];
	  ped_width_slboard.at(14-tmp_layer).at(tmp_chip).at(tmp_channel).at(isca)=tmp_width[isca];
	}
      }
    }

  }

}
