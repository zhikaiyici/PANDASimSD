
#include "UserDataInput.hh"

#include "G4SystemOfUnits.hh"

#include <fstream>
#include <iomanip>

using namespace std;

G4int UserDataInput::threadNumber = 4;
G4int UserDataInput::eventNumber = 1000;
G4int UserDataInput::arraySize = 3;

G4bool UserDataInput::uiStatus = false;
G4bool UserDataInput::opticalPhysics = true;

G4double UserDataInput::distanceBetweenModules = 2.;
G4double UserDataInput::gdFilmThickness = 30.;
G4double UserDataInput::dtctrX = 10.;
G4double UserDataInput::dtctrY = 10.;
G4double UserDataInput::dtctrZ = 100.;
G4double UserDataInput::neutrinoPercentage = 1.;

G4String UserDataInput::sourceType = "NEUTRINO";
G4String UserDataInput::sourcePosition = "CENTER";

vector<G4double> UserDataInput::positronEnergy = *(new vector<G4double>); //粒子能量MeV
vector<G4double> UserDataInput::positronCDFSpectrum = *(new vector<G4double>); //归一化能谱
vector<G4double> UserDataInput::neutronEnergy = *(new vector<G4double>); //粒子能量MeV
vector<G4double> UserDataInput::neutronCDFSpectrum = *(new vector<G4double>); //归一化能谱

array<G4int, 2> UserDataInput::neutrinoPosition = { arraySize * arraySize, 5 };

UserDataInput::UserDataInput()
{
}

UserDataInput::~UserDataInput()
{
}

void UserDataInput::ReadInputData()
{
	G4String prmtrFile = "InputSet.txt";
	ifstream prameterFile(prmtrFile, ios_base::in);		//输入文件
	if (prameterFile.good() != true)
		G4cout << "FAILED to open file " << prmtrFile << G4endl;
	else
	{
		G4cout << "File " << prmtrFile << " has been opened SUCCESSFULLY." << G4endl;

		G4String input;
		G4String positronSpectrumName; //能谱文件名
		G4String neutronSpectrumName; //中子能谱文件名

		while (prameterFile >> input)			//读取参数
		{
			if (input == "Optical_Physics:")
			{
				G4String Optical_Physics;
				prameterFile >> Optical_Physics;
				if (Optical_Physics == "ON")
				{
					opticalPhysics = true;
					G4cout << "The optical physics is ON." << G4endl;
				}
				else
				{
					opticalPhysics = false;
					G4cout << "The optical physics is OFF." << G4endl;
				}
			}
			else if (input == "Status_Of_UI:")
			{
				G4String Status_Of_UI;
				prameterFile >> Status_Of_UI;
				if (Status_Of_UI == "ON")
				{
					uiStatus = true;
				}
				else
				{
					uiStatus = false;
				}
			}
			else if (input == "Number_Of_Threads:")
			{
				prameterFile >> threadNumber;
			}
			else if (input == "Number_Of_Events:")
			{
				G4double doubNumberOfEvents;
				prameterFile >> doubNumberOfEvents;
				eventNumber = (G4int)doubNumberOfEvents;
				G4cout << doubNumberOfEvents << " events will be simulated." << G4endl;
			}
			else if (input == "Size_Of_Array:")
			{
				prameterFile >> arraySize;
				G4cout << "The size of array is " << arraySize << " x " << arraySize << G4endl;
			}
			else if (input == "Tickness_Of_Gd_Film:")
			{
				prameterFile >> gdFilmThickness;
				G4cout << "The Gd film thickness is " << gdFilmThickness << " um." << G4endl;
			}
			else if (input == "XYZ:")
			{
				prameterFile >> dtctrX >> dtctrY >> dtctrZ;
				G4cout << "The dimension of detector is " << dtctrX << " cm x " << dtctrY << " cm x " << dtctrZ << " cm" << G4endl;
			}
			else if (input == "Distance_Between_Modules:")
			{
				prameterFile >> distanceBetweenModules;
				G4cout << "The distance between modules is " << distanceBetweenModules << " cm." << G4endl;
			}
			else if (input == "Type_Of_Source:")
			{
				prameterFile >> sourceType >> sourcePosition;
				G4cout << sourceType << " source.";
				if (sourceType != "NEUTRINO" && sourceType != "COSMICNEUTRON" && sourceType != "CRY" && sourceType != "He8" && sourceType != "Li9")
				{
					G4cout << " At " << sourcePosition << ".";
				}
				G4cout << G4endl;
			}
			else if (input == "Positron_Spectrum_File_Name:")
			{
				prameterFile >> positronSpectrumName;
			}
			else if (input == "Neutron_Spectrum_File_Name:")
			{
				prameterFile >> neutronSpectrumName;
			}
			else if (input == "Percentage_Of_Neutrino:")
			{
				if (sourceType == "NEUTRINO")
				{
					prameterFile >> neutrinoPercentage;
					G4cout << "The Percentage_Of_Neutrino is " << neutrinoPercentage << G4endl;
				}
			}
			else if (input == "Position_Of_Neutrino:")
			{
				if (sourceType == "NEUTRINO")
				{
					prameterFile >> neutrinoPosition[0] >> neutrinoPosition[1];
					G4cout << "The Position_Of_Neutrino is " << neutrinoPosition[0] << "  " << neutrinoPosition[1] << G4endl;
				}
			}
		}
		prameterFile.close();

		ReadSpectra(positronSpectrumName, positronEnergy, positronCDFSpectrum);
		ReadSpectra(neutronSpectrumName, neutronEnergy, neutronCDFSpectrum);
	}
}

void UserDataInput::ReadSpectra(G4String spectrumName, vector<G4double>& energy, vector<G4double>& cdfSpectrum)
{
	ifstream spcFile;
	spectrumName = "spectra/" + spectrumName;

	spcFile.open(spectrumName, ios_base::in);
	if (spcFile.good() != true)
		G4cout << "FAILED to open spectrum file " << spectrumName << G4endl;
	else
	{
		G4cout << "Spectrum file " << spectrumName << " has been opened SUCCESSFULLY." << G4endl;

		//读取能谱,存入energy和spectrum中
		unsigned int i = 0;
		G4double temp, sum = 0;
		vector<G4double> spectrum;
		while (spcFile >> temp)
		{
			energy.push_back(temp);
			spcFile >> temp;
			spectrum.push_back(temp);
			i++;
		}
		spcFile.close();

		//归一化能谱
		for (i = 0; i < energy.size(); ++i)
			sum += spectrum[i]; //求能谱总和
		vector<G4double> normSpectrum;
		for (i = 0; i < energy.size(); ++i)
			normSpectrum.push_back(spectrum[i] / sum);  //归一化谱数据 
		cdfSpectrum.push_back(normSpectrum[0]);
		for (i = 1; i < energy.size(); ++i)
			cdfSpectrum.push_back(normSpectrum[i] + cdfSpectrum[i - 1]);
	}
}
