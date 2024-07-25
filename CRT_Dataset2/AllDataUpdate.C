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
#include <TStyle.h>
#include <TMath.h>
#include <TH2.h>
#include <TLine.h>
double gaussian(double *x, double *par) {
	double arg = (x[0] - par[1]) / par[2];
    	return par[0] * exp(-0.5 * arg * arg);
}


double calculate_mean(const std::vector<double>& data) {
	double sum = 0.0;
    	for (double value : data) {
        	sum += value;
    	}
    	return sum / data.size();
	//return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}
double calculate_stddev(const std::vector<double>& data, double mean) {
	double sum = 0.0;
    	for (double value : data) {
        	sum += (value - mean) * (value - mean);
    	}
    	return sqrt(sum / data.size());
}
double gaussian2D(double *x, double *par) {
	double argX = (x[0] - par[1]) / par[2];
    	double argY = (x[1] - par[3]) / par[4];
    	return par[0] * exp(-0.5 * (argX * argX + argY * argY));
	
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

	gStyle->SetOptStat(0);
	double entries = histogram->GetEntries();
	std::cout << "Entries: " << entries << std::endl;
	
	fit_func->SetLineColor(kRed);
	fit_func->SetLineWidth(2);
	fit_func->Draw("same");

}
  

void fit_gaussian2D(TH2F* histogram) {
	int n_binsX = histogram->GetNbinsX();
    	int n_binsY = histogram->GetNbinsY();
    	std::vector<double> dataX;
    	std::vector<double> dataY;
	//double faulty_x_min = -360;
	//double faulty_x_max = 0;
    	//double faulty_y_min = 0;
    	//double faulty_y_max = 72;

	for (int i = 1; i <= n_binsX; ++i) {
        	for (int j = 1; j <= n_binsY; ++j) {
            		double bin_centerX = histogram->GetXaxis()->GetBinCenter(i);
            		double bin_centerY = histogram->GetYaxis()->GetBinCenter(j);
            	//	if (bin_centerX >= faulty_x_min && bin_centerX <= faulty_x_max &&
                //		bin_centerY >= faulty_y_min && bin_centerY <= faulty_y_max) {
                //		histogram->SetBinContent(i, j, 0);
            	//	} else {
               		int bin_count = histogram->GetBinContent(i, j);
               		for (int k = 0; k < bin_count; ++k) {
                 		dataX.push_back(bin_centerX);
                    		dataY.push_back(bin_centerY);
                	}
            	}
        }
    	


	double meanX = calculate_mean(dataX);
	std::cout << "MeanX = " << meanX << std::endl;
    	double meanY = calculate_mean(dataY);
	std::cout << "MeanY = " << meanY << std::endl;
    	double stddevX = calculate_stddev(dataX, meanX);
	std::cout << "stddevX = " << stddevX << std::endl;
    	double stddevY = calculate_stddev(dataY, meanY);
	std::cout << "stddevY = " << stddevY << std::endl;

    	TF2 *fit_func = new TF2("fit_func", gaussian2D, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), histogram->GetYaxis()->GetXmin(), histogram->GetYaxis()->GetXmax(), 5);
    	fit_func->SetParameters(histogram->GetMaximum(), meanX, stddevX, meanY, stddevY);
    	histogram->Fit(fit_func, "N");
	
    	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	std::cout << "#chi^{2} = " << chi2 << std::endl;
    	std::cout << "NDF = " << ndf << std::endl;

	gStyle->SetOptStat(0);
	double entries = histogram->GetEntries();
	std::cout << "Entries: " << entries << std::endl;	

}    

void AllDataUpdate() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13688_crtana_22jul2024.root");
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13689_crtana_22jul2024.root");
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13690_crtana_22jul2024.root");
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13693_crtana_22jul2024.root");
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13758_crtana_22jul2024.root"); // Replace with your file path pattern
	TH1F *histogram1_f_nb = new TH1F("histogram1_f_nb", "Front Face X;No Beam", 10, -360, 360);
	histogram1_f_nb->GetXaxis()->SetTitle("X Location (cm)");
    	histogram1_f_nb->GetYaxis()->SetTitle("Number of Hits");

	TH1F *histogram2_f_nb = new TH1F("histogram2_f_nb", "Front Face Y;No Beam", 10, -360, 360);
	histogram2_f_nb->GetXaxis()->SetTitle("Y Location (cm)");
        histogram2_f_nb->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogram3_f_nb = new TH2F("histogram3_f_nb", "Front Face XY;No Beam", 10, -360, 360, 10, -360, 360);
	histogram3_f_nb->GetXaxis()->SetTitle("X Location (cm)");
        histogram3_f_nb->GetYaxis()->SetTitle("Y Location (cm)");

	TH1F *histogram4_f_tc = new TH1F("histogram4_f_tc", "X Axis of the CRT Detector Activity on the Front Face", 10, -360, 360);
        histogram4_f_tc->GetXaxis()->SetTitle("X Location (cm)");
        histogram4_f_tc->GetYaxis()->SetTitle("Number of Hits");

	TH1F *histogram5_f_tc = new TH1F("histogram5_f_tc", "Y Axis of the CRT Detector Activity on the Front Face", 10, -360, 360);
        histogram5_f_tc->GetXaxis()->SetTitle("Y Location (cm)");
        histogram5_f_tc->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogram6_f_tc = new TH2F("histogram6_f_tc", "2D Visualization of the Detector Activity on the CRT Front Face", 10, -360, 360, 10, -360, 360);
	histogram6_f_tc->GetXaxis()->SetTitle("X Location (cm)");
        histogram6_f_tc->GetYaxis()->SetTitle("Y Location (cm)");
	//histogram6_f_tc->SetContour(contours.size(), contours.data());

    	TCanvas *c1_f_nb = new TCanvas("c1_f_nb", "Front Face X NB");
	TCanvas *c2_f_nb = new TCanvas("c2_f_nb", "Front Face Y NB");
	TCanvas *c3_f_nb = new TCanvas("c3_f_nb", "Front Face XY NB");
	TCanvas *c4_f_tc = new TCanvas("c4_f_tc", "Front Face X TC");
        TCanvas *c5_f_tc = new TCanvas("c5_f_tc", "Front Face Y TC");
        TCanvas *c6_f_tc = new TCanvas("c6_f_tc", "Front Face XY TC");
	TCanvas *c7_f_ob = new TCanvas("c7_f_ob", "Detector Front Face X OB");
        TCanvas *c8_f_ob = new TCanvas("c8_f_ob", "Front Face Y OB");
        TCanvas *c9_f_ob = new TCanvas("c9_f_ob", "Front Face XY OB");
	TCanvas *c10_f_nb_scaled = new TCanvas("c10_f_nb_scaled", "Front Face X NB Scaled");
    	TCanvas *c11_f_nb_scaled = new TCanvas("c11_f_nb_scaled", "Front Face Y NB Scaled");
    	TCanvas *c12_f_nb_scaled = new TCanvas("c12_f_nb_scaled", "Front Face XY NB Scaled");



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

	gStyle->SetOptStat(0);
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

        		if (1530e3 < t1 && t1 < 1533e3) {
                		if (y > -360 && -360 < x && x < 360) {
                    			if (-200 < z && z < -100) {
                        			histogram4_f_tc->Fill(x);
                        			histogram5_f_tc->Fill(y);
                        			histogram6_f_tc->Fill(x, y);
                   		 	}
               		 	}
           	 	}
			if (1527e3 < t1 && t1 < 1530e3) { // || (1533e3 < t1 && t1 < 1536e3)) {
                		if (y > -360 && -360 < x && x < 360) {  // cut off feet of detector
                    			if (-200 < z && z < -100) {
                        			histogram1_f_nb->Fill(x);
                        			histogram2_f_nb->Fill(y);
                        			histogram3_f_nb->Fill(x, y);
                    			}
                		}
            		}
        	}
    	}

	double start_time = 0;
	double end_time = 1000e6;
	double start_spill = 1530e3;
	double end_spill = 1533e3;

	double total_time = end_time - start_time;
        double spill_time = end_spill - start_spill;
        double no_spill = total_time - spill_time;
        double weight = spill_time / no_spill;

//	histogram1_f_nb->Scale(weight);
//	histogram2_f_nb->Scale(weight);
//	histogram3_f_nb->Scale(weight);

	TH1F *histogramx = (TH1F*)histogram4_f_tc->Clone("histogramx");
        histogramx->Add(histogram1_f_nb, -1.0);
        histogramx->GetXaxis()->SetTitle("X Location (cm)");
        histogramx->GetYaxis()->SetTitle("Number of Hits");	
		
	TH1F *histogramy = (TH1F*)histogram5_f_tc->Clone("histogramy");
        histogramy->Add(histogram2_f_nb, -1.0);
	histogramy->GetXaxis()->SetTitle("Y Location (cm)");
        histogramy->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogramxy = (TH2F*)histogram6_f_tc->Clone("histogramxy");
        histogramxy->Add(histogram3_f_nb, -1.0);
	histogramxy->GetXaxis()->SetTitle("X Location (cm)");
        histogramxy->GetYaxis()->SetTitle("Y Location (cm)");
	//histogramxy->ClearContours();
	//histogramxy->SetContour(contours.size(), contours.data());

	c1_f_nb->cd();
    	histogram1_f_nb->Draw("HIST");
	fit_gaussian1D(histogram1_f_nb);
    	c2_f_nb->cd();
    	histogram2_f_nb->Draw("HIST");
	fit_gaussian1D(histogram2_f_nb);
    	c3_f_nb->cd();
    	histogram3_f_nb->Draw("COLZ");
	fit_gaussian2D(histogram3_f_nb);	
	//histogram6_f_tc->Draw("CONT1 SAME");

	c4_f_tc->cd();
	histogram4_f_tc->Draw("HIST");
	fit_gaussian1D(histogram4_f_tc);
	c5_f_tc->cd();
        histogram5_f_tc->Draw("HIST");
        fit_gaussian1D(histogram5_f_tc);
	c6_f_tc->cd();
        histogram6_f_tc->Draw("COLZ");
        fit_gaussian2D(histogram6_f_tc);
	c7_f_ob->cd();
        histogramx->Draw("HIST");
        fit_gaussian1D(histogramx);
	c8_f_ob->cd();
        histogramy->Draw("HIST");
        fit_gaussian1D(histogramy);
	c9_f_ob->cd();
        histogramxy->Draw("COLZ");
        fit_gaussian2D(histogramxy);

//	c10_f_nb_scaled->cd();
//    	histogram1_f_nb->Draw("HIST");
//    	c11_f_nb_scaled->cd();
//    	histogram2_f_nb->Draw("HIST");
    	//c12_f_nb_scaled->cd();
    	//histogram3_f_nb->Draw("COLZ");

//	TCanvas *c_overlay = new TCanvas("c_overlay", "Y Face Time Cut Compared to Cosmic Ray Background");
//    	histogram5_f_tc->SetLineColor(kBlue);  // Set the color of the first histogram
//    	histogram5_f_tc->Draw("HIST");
//   	histogram2_f_nb->SetLineColor(kRed);  // Set the color of the second histogram
//    	histogram2_f_nb->SetLineStyle(2);
//	histogram2_f_nb->Draw("HIST SAME");  // Draw the second histogram on the same canvas



//	c1_f_nb->SaveAs("NewData_Front_face_x_nb.png");
//    	c2_f_nb->SaveAs("NewData_Front_face_y_nb.png");
//    	c3_f_nb->SaveAs("NewData_Front_face_nb.png");
//	c4_f_tc->SaveAs("NewData_Front_face_x_tc.png");
//        c5_f_tc->SaveAs("NewData_Front_face_y_tc.png");
//        c6_f_tc->SaveAs("NewData_Front_face_tc.png");
	c7_f_ob->SaveAs("NewData_Front_face_x_ob.png");
        c8_f_ob->SaveAs("NewData_Front_face_y_ob.png");
        c9_f_ob->SaveAs("NewData_Front_face_ob.png");
//	c10_f_nb_scaled->SaveAs("Scaled_Front_face_x_nb.png");
//	c11_f_nb_scaled->SaveAs("Scaled_Front_face_y_nb.png");
//	c_overlay->SaveAs("NewData_Y_overlay.png");
}

