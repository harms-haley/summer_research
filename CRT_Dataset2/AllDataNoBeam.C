#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>
#include "TF1.h"
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include "TPaveText.h"
#include "TF2.h"


double gaussian(double *x, double *par) {
	double arg = (x[0] - par[1]) / par[2];
    	return par[0] * exp(-0.5 * arg * arg);
}

double gaussian2D(double *x, double *par) {
	double argX = (x[0] - par[1]) / par[2];
    	double argY = (x[1] - par[3]) / par[4];
    	return par[0] * exp(-0.5 * (argX * argX + argY * argY));
}


double calculate_mean(const std::vector<double>& data) {
	return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}
double calculate_stddev(const std::vector<double>& data, double mean) {
	double sum = 0.0;
    	for (double value : data) {
        	sum += (value - mean) * (value - mean);
    	}
    	return sqrt(sum / data.size());
}

void fit_gaussian1D(TH1F* histogram) {
	int n_bins = histogram->GetNbinsX();
    	std::vector<double> data;
    	for (int i = 1; i <= n_bins; ++i) {
        	int bin_count = histogram->GetBinContent(i);
        	double bin_center = histogram->GetBinCenter(i);
        	for (int j = 0; j < bin_count; ++j) {
            		data.push_back(bin_center);
        	}
    	}	
	double mean = calculate_mean(data);
    	double stddev = calculate_stddev(data, mean);

    	TF1 *fit_func = new TF1("fit_func", gaussian, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), 3);
    	fit_func->SetParameters(histogram->GetMaximum(), mean, stddev);
    	histogram->Fit(fit_func, "R");
	
	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	TPaveText *pt = new TPaveText(0.6, 0.7, 0.9, 0.9, "NDC");
    	pt->SetFillColor(0);
    	pt->SetTextAlign(12);
    	pt->AddText(Form("#chi^{2} = %.2f", chi2));
    	pt->AddText(Form("NDF = %d", ndf));
    	pt->Draw();

    	histogram->GetListOfFunctions()->Add(pt);
}

void fit_gaussian2D(TH2F* histogram) {
	int n_binsX = histogram->GetNbinsX();
    	int n_binsY = histogram->GetNbinsY();
    	std::vector<double> dataX;
    	std::vector<double> dataY;
    	for (int i = 1; i <= n_binsX; ++i) {
        	for (int j = 1; j <= n_binsY; ++j) {
            		int bin_count = histogram->GetBinContent(i, j);
            		double bin_centerX = histogram->GetXaxis()->GetBinCenter(i);
            		double bin_centerY = histogram->GetYaxis()->GetBinCenter(j);
            		for (int k = 0; k < bin_count; ++k) {
                		dataX.push_back(bin_centerX);
                		dataY.push_back(bin_centerY);
            		}
        	}
    	}

	double meanX = calculate_mean(dataX);
    	double meanY = calculate_mean(dataY);
    	double stddevX = calculate_stddev(dataX, meanX);
    	double stddevY = calculate_stddev(dataY, meanY);

    	TF2 *fit_func = new TF2("fit_func", gaussian2D, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), histogram->GetYaxis()->GetXmin(), histogram->GetYaxis()->GetXmax(), 5);
    	fit_func->SetParameters(histogram->GetMaximum(), meanX, stddevX, meanY, stddevY);
    	histogram->Fit(fit_func, "R");

    	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	TPaveText *pt = new TPaveText(0.6, 0.7, 0.9, 0.9, "NDC");
    	pt->SetFillColor(0);
    	pt->SetTextAlign(12);
    	pt->AddText(Form("#chi^{2} = %.2f", chi2));
    	pt->AddText(Form("NDF = %d", ndf));
    	pt->Draw();

    	histogram->GetListOfFunctions()->Add(pt);
}
void AllDataNoBeam() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13*_crtana.root"); // Replace with your file path pattern
	TH1F *histogram1_f_nb = new TH1F("histogram1_f_nb", "Front Face X;No Beam", 10, -400, 400);
	TH1F *histogram2_f_nb = new TH1F("histogram2_f_nb", "Front Face Y;No Beam", 10, -400, 400);
	TH1F *histogram4_f_nb = new TH1F("histogram4_f_nb", "Time", 10, 1520e3 , 1540e3);
	TH2F *histogram3_f_nb = new TH2F("histogram3_f_nb", "Front Face XY;No Beam", 10, -400, 400, 10, -400, 400);
    	TCanvas *c1_f_nb = new TCanvas("c1_f_nb", "Front Face X");
	TCanvas *c2_f_nb = new TCanvas("c2_f_nb", "Front Face Y");
	TCanvas *c3_f_nb = new TCanvas("c3_f_nb", "Front Face XY");
	TCanvas *c4_f_nb = new TCanvas("c4_f_nb", "Time");

	std::vector<double> *cl_sp_x = nullptr;
    	std::vector<double> *cl_sp_y = nullptr;
    	std::vector<double> *cl_sp_z = nullptr;
    	std::vector<double> *cl_sp_ts1 = nullptr;
    	std::vector<bool> *cl_has_sp = nullptr;
	chain.SetBranchAddress("cl_has_sp", &cl_has_sp);
    	chain.SetBranchAddress("cl_sp_x", &cl_sp_x);
    	chain.SetBranchAddress("cl_sp_y", &cl_sp_y);
    	chain.SetBranchAddress("cl_sp_z", &cl_sp_z);
    	chain.SetBranchAddress("cl_sp_ts1", &cl_sp_ts1);

	Long64_t nEntries = chain.GetEntries();

	for (Long64_t i = 0; i < nEntries; ++i) {
        	chain.GetEntry(i);
		if (cl_has_sp == nullptr || cl_has_sp->size() == 0 || !cl_has_sp->at(0)) {
            		continue;
        	}
		
		for (size_t j = 0; j < cl_sp_x->size(); ++j) {
            		double t1 = cl_sp_ts1->at(j);
            		double x = cl_sp_x->at(j);
            		double y = cl_sp_y->at(j);
            		double z = cl_sp_z->at(j);
			if (i % 10000 == 0) {
				std::cout << "Entry " << i << ", Hit " << j << ": t1=" << t1 << ", x=" << x << ", y=" << y << ", z=" << z << std::endl;
			}
			
			histogram4_f_nb->Fill(t1);
			
			if (t1 < 1529e3 || t1 > 1534e3) {
				if (y > -350 && -370 < x && x < 370) {  // cut off feet of detector
                    			if (-200 < z && z < -100) {
                        			histogram1_f_nb->Fill(x);
                        			histogram2_f_nb->Fill(y);
                        			histogram3_f_nb->Fill(x, y);
                    			}
                		}
            		}
        	}
    	}


	c1_f_nb->cd();
    	histogram1_f_nb->Draw();
	//fit_gaussian1D(histogram1_f_t);
    	c2_f_nb->cd();
    	histogram2_f_nb->Draw();
	//fit_gaussian1D(histogram2_f_t);
    	c3_f_nb->cd();
    	histogram3_f_nb->Draw("COLZ");
	//fit_gaussian2D(histogram3_f_t);	
	c4_f_nb->cd();
	histogram4_f_nb->Draw();

	c1_f_nb->SaveAs("Front_face_x_nb_all.png");
    	c2_f_nb->SaveAs("Front_face_y_nb_all.png");
    	c3_f_nb->SaveAs("Front_face_nb_all.png");
	c4_f_nb->SaveAs("Time.png");

}

