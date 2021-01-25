
// Privateer (PRogramatic Identification of Various Anomalies Toothsome Entities Experience in Refinement)
//    -> The name combines Mark Knopfler's 'Privateering' cracking album & Dr Cowtan's tradition of using maritime acronyms
// Licence: LGPL (https://www.gnu.org/licenses/lgpl.html)
//
// 2013-2017 Jon Agirre & Kevin Cowtan
// York Structural Biology Laboratory
// The University of York
// mailto: jon.agirre@york.ac.uk
// mailto: kevin.cowtan@york.ac.uk
//
// Targets supported: structures containing known or pre-defined furanose and pyranose rings
//
// The program, initially conceived for simple conformational validation, has evolved into a sort of
// Swiss-army knife, supporting re-refinement of sugars and graphical analysis of glycans through SVG graphics
//
// Some of it's features are:
//
//   - Cremer-pople puckering parameters for pyranose and furanose rings
//   - Comparison between detected and minimal-energy conformation
//   - Glycochemical validation: Privateer will detect wrong links and nomenclature
//   - Linkage torsion calculation and annotation
//   - Annotated SVG graphics files for N-, O- and S-glycans
//   - Generation of dictionaries containing aperiodic torsions (for re-refinement)
//   - Python interface for connecting to other programs (CCP4i2, CCP4mg and Coot)
//   - Coot-friendly output through Python and Scheme scripts
//   - SFcalc with bulk solvent correction from model (internal)
//   - Fo-Fc sigmaa weighted omit maps (output as map file or map coeffs)
//   - Real Space Correlation coefficient + score (console and XML)
//   - Mean B-factors, <mFo> and other metrics

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <iostream>
#include <iomanip>
#include "privateer-lib.h"
#include "privateer-cryo_em.h"
#include "privateer-xray.h"
#include "clipper-glyco.h"
#include "privateer-blobs.h"
#include "privateer-composition.h"
#include "privateer-dbquery.h"
#include "privateer-parallelism.h"
#include <clipper/clipper.h>
#include <clipper/clipper-cif.h>
#include <clipper/clipper-mmdb.h>
#include <clipper/clipper-ccp4.h>
#include <clipper/clipper-contrib.h>
#include <clipper/clipper-minimol.h>
#include <clipper/contrib/sfcalc_obs.h>
#include <clipper/minimol/minimol_utils.h>

// #define DUMP 1
#define DBG std::cout << "[" << __FUNCTION__ << "] - "

clipper::String program_version = "MKIV_alpha";
using clipper::data32::F_sigF;
using clipper::data32::F_phi;
using clipper::data32::Phi_fom;
using clipper::data32::Flag;
typedef clipper::HKL_data_base::HKL_reference_index HRI;


// Glytoucan has to be the last arguement for some reason, need to fix this bs. Otherwise new arguements will not be picked up.

int main(int argc, char** argv)
{
    CCP4Program prog( "Privateer", program_version.c_str(), "$Date: 2020/03/02" );

    prog.set_termination_message( "Failed" );

    std::cout << "\nCopyright 2013-2019 Jon Agirre, Kevin Cowtan and The University of York." << std::endl  ;
    std::cout << "\n\nPlease reference these articles: "<< std::endl ;
    std::cout << "\n  'Privateer: software for the conformational validation of carbohydrate structures'";
    std::cout << "\n   Agirre J, Fernandez-Iglesias J, Rovira C, Davies GJ, Wilson KS and Cowtan KD. (2015) Nature Structural & Molecular Biology 22 (11), 833-834." << std::endl;
    std::cout << "\n  'Carbohydrate anomalies in the PDB'";
    std::cout << "\n   Agirre J, Davies GJ, Wilson KS and Cowtan KD. (2015) Nature Chemical Biology 11 (5), 303." << std::endl << std::endl;

    #if DUMP
        std::cout << "   WARNING: THIS IS A DEBUG VERSION - NOT INTENDED FOR PUBLIC DISTRIBUTION" << std::endl ;
    #endif

    clipper::HKL_info hklinfo; // allocate space for the hkl metadata
    clipper::CIFfile cifin;
    clipper::CCP4MTZfile mtzin, ampmtzin;
    clipper::CCP4MAPfile mrcin;
    clipper::Xmap<double> cryo_em_map;
    clipper::String input_model             = "NONE";
    clipper::String input_cryoem_map        = "NONE";
    clipper::String input_column_fobs       = "NONE";
    clipper::String input_reflections_cif   = "NONE";
    clipper::String input_ccd_code          = "XXX";
    clipper::String ipwurcsjson             = "database.json";
    clipper::String output_mapcoeffs_mtz    = "privateer-hklout.mtz";
    clipper::String title                   = "generic title";
    clipper::String input_reflections_mtz   = "NONE";
    clipper::String input_expression_system = "undefined";
    clipper::String input_validation_string = "";
    std::vector<clipper::String> input_validation_options;
    clipper::data::sugar_database_entry external_validation;
    bool glucose_only = true;
    bool useSigmaa = false;
    bool oldstyleinput = false;
    bool vertical = false, original = true, invert = false;
    int n_refln = 1000;
    int n_param = 20;
    int sleepTimer = 1;
    bool clean_output = false;
    unsigned int nThreads = 0;
    bool useMTZ = false;
    bool useMRC = false;
    bool batch = false;
    bool noMaps = false;
    bool allSugars = true;
    bool showGeom = false;
    bool check_unmodelled = false;
    bool ignore_set_null = false;
    bool useWURCSDataBase = false;
    bool useParallelism = true;
    float resolution = -1; 
    float ipradius = 2.5;    // default value, punishing enough!
    float thresholdElectronDensityValue = 0.02;
    FILE *output;
    bool output_mtz = false;
    std::vector < clipper::MGlycan > list_of_glycans;
    std::vector<std::vector<std::pair<std::pair<clipper::MGlycan, std::vector<int>>,float>>> list_of_glycans_associated_to_permutations;
    clipper::CCP4MTZfile opmtz_best, opmtz_omit;
    clipper::MTZcrystal opxtal;
    clipper::MTZdataset opdset;
    clipper::MGlycology mgl;
    nlohmann::json jsonObject;
    privateer::thread_pool pool(0);



    // command input
    CCP4CommandInput args( argc, argv, true );
    int arg = 0;
    while ( ++arg < args.size() )
    {
        if ( args[arg] == "-title" )
        {
            if ( ++arg < args.size() )
                title = args[arg];
        }
        else if ( args[arg] == "-colin-fo" )
        {
            if  (++arg < args.size() )
                input_column_fobs = args[arg];
        }
        else if ( args[arg] == "-mtzout" )
        {
            if ( ++arg < args.size() )
            {
                output_mapcoeffs_mtz = args[arg];
                output_mtz = true;
            }
        }
        else if ( args[arg] == "-pdbin" )
        {
            if ( ++arg < args.size() )
                input_model = args[arg];
        }
        else if ( args[arg] == "-mtzin" )
        {
            if ( ++arg < args.size() )
            {
                useMTZ = true;
                input_reflections_mtz = args[arg];
            }
        }
        else if ( args[arg] == "-mapin" )
        {
            if ( ++arg < args.size() )
            {
                useMRC = true;
                input_cryoem_map = args[arg];
            }
        }
        else if ( args[arg] == "-resolution" )
        {
            if ( ++arg < args.size() )
            {
                resolution = clipper::String(args[arg]).f();
                if (resolution < 0.0)
                {
                    std::cout << "\n\nResolution value is negative! Expected a positive decimal." << std::endl << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
            }
        }
        else if ( args[arg] == "-cifin" )
        {
            if ( ++arg < args.size() )
                input_reflections_cif = args[arg];
        }
        else if ( args[arg] == "-list" )
        {
            privateer::util::print_supported_code_list();
            prog.set_termination_message( "Success" );
            return 0;
        }
        else if ( args[arg] == "-expression" )
        {
            if ( ++arg < args.size() )
                input_expression_system = args[arg];
            if ( input_expression_system.trim() != "undefined" &&
                 input_expression_system.trim() != "fungal" &&
                 input_expression_system.trim() != "yeast" &&
                 input_expression_system.trim() != "plant" &&
                 input_expression_system.trim() != "insect" &&
                 input_expression_system.trim() != "mammalian" &&
                 input_expression_system.trim() != "human" )
            {
                std::cout << std::endl << std::endl << "Error: " << input_expression_system << " is not a supported expression system" << std::endl << std::endl;
                privateer::util::print_usage();
                prog.set_termination_message( "Failed" );
                return 1;
            }
        }
        else if ( args[arg] == "-codein" )
        {
            if ( ++arg < args.size() )
            {
                input_ccd_code = clipper::String(args[arg]);
                allSugars = false;

                if (input_ccd_code.trim().length() != 3)
                {
                    std::cout << std::endl << std::endl << "Error: the sugar code must be three characters long, e.g. GLC."
                    << "\nPlease refer to the Chemical Component Dictionary (http://www.wwpdb.org/ccd.html).\n"
                    << "Alternatively, use the -list option to get a full list of supported codes\nExiting..." << std::endl << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
            }
        }
        else if ( args[arg] == "-all_permutations" )
        {
            glucose_only = false;
        }
        else if ( args[arg] == "-cores" )
        {
            if ( ++arg < args.size() )
            {
                int detectedThreads = std::thread::hardware_concurrency();
                nThreads = clipper::String(args[arg]).i();
                
                if(nThreads < 2)
                {
                    std::cout << "Error: Less than 2 cores/threads were inputted as an argument." << "\nPlease disable multithreaded execution via -singlethreaded keyword argument or give more cores to Privateer!" << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
                
                if(nThreads > detectedThreads)
                {
                    std::cout << "Error: More cores/threads were inputted as an argument, than detected on the system." 
                    << "\n\tNumber of Available Cores/Threads detected on the system: " << detectedThreads 
                    << "\n\tNumber of Cores/Threads requested via -cores argument: " << nThreads << "." << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
            }
        }
        else if ( args[arg] == "-singlethreaded" )
        {
            useParallelism = false;
            nThreads = 0;
        }
        else if ( args[arg] == "-clean_output" )
        {
            clean_output = true;
        }
        else if ( args[arg] == "-glytoucan" )
        {
            useWURCSDataBase = true;
            // ipwurcsjson = "/home/harold/Dev/privateer_dev_noccp4/src/privateer/database.json";
            if ( ++arg < args.size() )
            {
                if(clipper::String(args[arg])[0] != '-')
                {
                    ipwurcsjson = args[arg];

                    std::string fileName = ipwurcsjson.tail();
                    std::string fileExtension = fileName.substr( fileName.length() - 5 );

                    if (fileExtension != ".json")
                    {
                        std::cout << std::endl << std::endl << "Error: the file input must be a .json!"
                        << "\nPlease make sure the path to .json file is correct!\nExiting..." << std::endl << std::endl;
                        prog.set_termination_message( "Failed" );
                        return 1;
                    }
                }
            }
        }
        else if ( args[arg] == "-sleep_timer" )
        {
            if ( ++arg < args.size() )
            {
                sleepTimer = clipper::String(args[arg]).i();
                if (sleepTimer < 1)
                {
                    std::cout << "Error: sleepTimer < 1." << "\nUnfortunately disabling sleepTimer is unadvisable due to some limitations in the parallelized code. Only use this argument to increase sleepTimer if you are getting segfaults in parallelized permutation algorithm!" << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
            }
        }
        else if ( args[arg] == "-mode" )
        {
            if ( ++arg < args.size() )
                if (clipper::String(args[arg]) == "ccp4i2")
                    batch = true;
        }
        else if ( args[arg] == "-vertical" )
        {
            vertical = true;
        }
        else if ( args[arg] == "-oldstyle" )
        {
            oldstyleinput = true;
        }
        else if ( args[arg] == "-essentials" )
        {
            original = false;
        }
        else if ( args[arg] == "-invert" )
        {
            invert = true;
        }
        else if ( args[arg] == "-radiusin" )
        {
            if ( ++arg < args.size() )
            {
                ipradius = clipper::String(args[arg]).f();
                if (ipradius < 1.0)
                {
                    std::cout << "\n\nMask radius is too small!" << std::endl << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
            }
        }
        else if ( args[arg] == "-valstring" )
        {
            if ( ++arg < args.size() )
            {
                input_validation_string = args[arg];

                input_validation_options = input_validation_string.split(",");
                if ( privateer::util::compute_and_print_external_validation ( input_validation_options, external_validation ) )
                {
                    prog.set_termination_message ( "Failed" );
                    return -1;
                }
            }
        }
        else if ( args[arg] == "-showgeom" )
            showGeom = true;

        else if ( args[arg] == "-nomaps" )
            noMaps = true;

        else if ( args[arg] == "-check-unmodelled" )
            check_unmodelled = true;


        else if ( args[arg] == "-ignore_missing" )
            ignore_set_null = true;


        else if ( args[arg] == "-blobs_threshold" )
        {
            if ( ++arg < args.size() )
            {
                thresholdElectronDensityValue = clipper::String(args[arg]).f();
            }
        }

        else
        {
            std::cout << "\nUnrecognised:\t" << args[arg] << std::endl;
            args.clear();
        }
    }

    if ( useMTZ && useMRC )
    {
        std::cout << "\nFATAL: Both MTZ and MRC file formats were inputted. Expected only one of them, not both at the same time!" << std::endl << std::endl;
        prog.set_termination_message("Failed");
        return 1;
    }

    if ( useMRC && resolution == -1)
    {
        std::cout << "\nFATAL: An MRC file was inputted, but no resolution value was given. To import a Cryo-EM map please use -mapin and -resolution arguments!" << std::endl << std::endl;
        prog.set_termination_message("Failed");
        return 1;
    }


    if ( input_ccd_code != "XXX" )
    {
        if ((( !clipper::MSugar::search_database(input_ccd_code.c_str())) && (clipper::MDisaccharide::search_disaccharides(input_ccd_code.c_str())==-1)) && (input_validation_options.size() == 0) )
        {
            std::cout << "\n\nError: no internal validation data found for " << input_ccd_code << std::endl;
            std::cout << "\nYou can provide external validation data with -valstring <data>" << std::endl;
            std::cout << "\n\tAccepted format: SUG,O5/C1/C2/C3/C4/C5,A,D,4c1\n";
            std::cout << "\tThree-letter code, ring atoms separated by /, anomer, handedness, expected conformation.\n" << std::endl;
            prog.set_termination_message("Failed");
            return 1;
        }
    }

    
    if (batch)
    {
        output = fopen("validation_data-privateer","w");
        if (NULL == output)
        {
            std::cout << std::endl << "Error: unable to create output file. Please check directory permissions." << std::endl;
            prog.set_termination_message( "Failed" );
            return 1;
        }
    }

    if ( (input_model != "NONE") && ((input_reflections_cif == "NONE") && (input_reflections_mtz == "NONE") && (input_cryoem_map == "NONE")) )
        noMaps = true;

    if ( (input_model == "NONE") || ((input_reflections_cif == "NONE") && (input_reflections_mtz == "NONE") && (input_cryoem_map == "NONE") && (noMaps == false)) )
    {
        privateer::util::print_usage();
        prog.set_termination_message( "Failed" );
        return 1;
    }

    unsigned int detectedCores;
    
    if(nThreads == 0 && useParallelism) nThreads = std::thread::hardware_concurrency(); 
    
    if(nThreads < 2 && useParallelism)
    {
        useParallelism = false;
        std::cout << std::endl << "Error: Less than two cores/threads were detected in the system. Number of Threads detected on the system: " << nThreads << "\nPlease disable multithreaded execution via -singlethreaded keyword argument!" << std::endl;
        prog.set_termination_message( "Failed" );
        return 1;
    }
    else if(!useParallelism)
        std::cout << std::endl << "THREADING: Running Privateer on a single thread (-singlethreaded argument was provided)!" << std::endl << std::endl;
    else
    {
        std::cout << std::endl << "THREADING: Resizing and initiating a thread pool object with " << nThreads << " threads..." << std::endl;
        pool.resize(nThreads);
        std::cout << "THREADING: Successfully initialized a thread pool with " << pool.size() << " threads!" << std::endl << std::endl;
    }
        


    clipper::MMDBfile mfile;
    clipper::MiniMol mmol;


    if ( (useMTZ && !useMRC) || noMaps ) privateer::util::read_coordinate_file_mtz ( mfile, mmol, input_model, batch);
    int pos_slash = input_model.rfind("/");

    if(useWURCSDataBase)
    {
        privateer::util::read_json_file (ipwurcsjson, jsonObject);
    }

    // Fast mode, no maps nor correlation calculations

    if ( noMaps )
    {
        clipper::Atom_list mainAtoms;
        clipper::Atom_list ligandAtoms;
        clipper::Atom_list allAtoms;
        std::vector< std::string > enable_torsions_for;

        if (!batch)
        {
            std::cout << std::endl << "Analysing carbohydrates... \n";
            fflush(0);
        }

        std::vector < std::pair <clipper::String , clipper::MSugar> > ligandList; // we store the Chain ID and create an MSugar to be scored
        std::vector < clipper::MMonomer > sugarList; // store the original MMonomer

        const clipper::MAtomNonBond& manb = clipper::MAtomNonBond( mmol, 1.0 ); // was 1.0

        mgl = clipper::MGlycology(mmol, manb, input_expression_system);

        list_of_glycans = mgl.get_list_of_glycans();
        list_of_glycans_associated_to_permutations.resize(list_of_glycans.size());

        if ( !batch ) std::cout << std::endl << "Number of detected glycosylations: " << list_of_glycans.size();

        if ( list_of_glycans.size() > 0 )
        {
            int glycansPermutated = 0;
            clipper::String current_chain = "" ;

            for (int i = 0; i < list_of_glycans.size() ; i++ )
            {  
                clipper::String wurcs_string;
                if ( current_chain != list_of_glycans[i].get_chain() )
                {
                    current_chain = list_of_glycans[i].get_chain();
                    std::cout << std::endl << std::endl << "Chain " << current_chain[0] << std::endl << "-------" << std::endl ;
                }
                std::cout << std::endl << list_of_glycans[i].print_linear ( true, false, true ) << std::endl;

                wurcs_string = list_of_glycans[i].generate_wurcs();
                std::cout << wurcs_string << std::endl;

                if(useWURCSDataBase)
                {
                    std::vector<std::pair<std::pair<clipper::MGlycan, std::vector<int>>,float>> finalGlycanPermutationContainer;
                    output_dbquery(jsonObject, wurcs_string, list_of_glycans[i], finalGlycanPermutationContainer, glucose_only, clean_output, sleepTimer, pool, useParallelism);

                    if (useParallelism)
                    {
                        #if DUMP
                            std::cout << std::endl;
                            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
                        #endif
                        
                        while(pool.n_remaining_jobs() > 0)
                            pool.sync();
                        
                        #if DUMP
                            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
                        #endif
                    }
                    
                    if(!finalGlycanPermutationContainer.empty())
                        {
                            glycansPermutated++;
                            list_of_glycans_associated_to_permutations.at(i) = finalGlycanPermutationContainer;
                            for(int j = 0; j < finalGlycanPermutationContainer.size(); j++)
                                {   
                                    if(oldstyleinput) 
                                    {
                                        privateer::glycoplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                                        plot.plot_glycan ( finalGlycanPermutationContainer[j].first.first );
                                        std::ostringstream os;
                                        os << finalGlycanPermutationContainer[j].first.first.get_root_for_filename() << "-" << j << "-PERMUTATION.svg";
                                        plot.write_to_file ( os.str() );
                                    }
                                    else
                                    {
                                        privateer::glycanbuilderplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                                        plot.plot_glycan ( finalGlycanPermutationContainer[j].first.first );
                                        std::ostringstream os;
                                        os << finalGlycanPermutationContainer[j].first.first.get_root_for_filename() << "-" << j << "-PERMUTATION.svg";
                                        plot.write_to_file ( os.str() );
                                    }
                                }
                        }
                }
                if(oldstyleinput)
                {
                    privateer::glycoplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                    plot.plot_glycan ( list_of_glycans[i] );
                    std::ostringstream os;
                    os << list_of_glycans[i].get_root_for_filename() << ".svg";
                    plot.write_to_file ( os.str() );
                }
                else
                {
                    privateer::glycanbuilderplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                    plot.plot_glycan ( list_of_glycans[i] );
                    std::ostringstream os;
                    os << list_of_glycans[i].get_root_for_filename() << ".svg";
                    plot.write_to_file ( os.str() );
                }
            }
            if(useWURCSDataBase && glycansPermutated > 0) std::cout << "Originally modelled glycans not found on GlyConnect database: " << glycansPermutated << "/" << list_of_glycans.size() << std::endl;
        }

        if ( !batch ) std::cout << "\n\nDetailed validation data" << std::endl;
        if ( !batch ) std::cout << "------------------------" << std::endl;

        // erase ligand atoms from the model and then calculate phases using
        // the omitted model, effectively computing an omit map

        for ( int p = 0; p < mmol.size(); p++ )
        {
            for ( int m = 0; m < mmol[p].size(); m++ )
            {
                if (allSugars)
                {
                    if ( clipper::MDisaccharide::search_disaccharides(mmol[p][m].type().c_str()) != -1 ) // treat disaccharide
                    {
                        clipper::MDisaccharide md(mmol, manb, mmol[p][m] );
                        sugarList.push_back ( mmol[p][m] );
                        sugarList.push_back ( mmol[p][m] );
                        clipper::String id = mmol[p].id();
                        id.resize(1);

                        ligandList.push_back ( std::pair < clipper::String, clipper::MSugar> (id, md.get_first_sugar()));
                        ligandList.push_back ( std::pair < clipper::String, clipper::MSugar> (id, md.get_second_sugar()));
                    }
                    else if ( !clipper::MSugar::search_database(mmol[p][m].type().c_str()) ) // true if strings are different
                    {
                        for (int id = 0; id < mmol[p][m].size(); id++ )
                        {
                            mainAtoms.push_back(mmol[p][m][id]); // cycle through atoms and copy them
                            allAtoms.push_back(mmol[p][m][id]);
                        }
                    }
                    else // it's one of the sugars contained in the database
                    {
                        clipper::MSugar msug, msug_b;

                        std::vector <char> conformers = privateer::util::number_of_conformers(mmol[p][m]);

                        // #if DUMP
                        //     std::cout << "number of alternate conformations: " << conformers.size() << std::endl;
                        // #endif

                        int n_conf = conformers.size();

                        if ( n_conf > 0 )
                        {
                            if ( n_conf == 1 )
                                msug   = clipper::MSugar(mmol, mmol[p][m], manb, conformers[0]);
                            else
                            {
                                msug   = clipper::MSugar(mmol, mmol[p][m], manb, conformers[0]);
                                msug_b = clipper::MSugar(mmol, mmol[p][m], manb, conformers[1]);
                            }

                        }
                        else
                        {
                            msug = clipper::MSugar(mmol, mmol[p][m], manb);
                        }

                        sugarList.push_back(mmol[p][m]);
                        clipper::String id = mmol[p].id();
                        id.resize(1);

                        #if DUMP
                            std::cout << "Looking at chain " << id << std::endl;
                        #endif

                        ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (id, msug));
                        // add both conformers if the current monomer contains more than one
                        if ( n_conf == 2 )
                            ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (id, msug_b));

                        if ( msug.type_of_sugar() == "unsupported" )
                        {
                            std::cout << std::endl << "Error: at least one of the sugars in the supplied PDB file is missing expected atoms"
                                      << std::endl << std::endl;
                            prog.set_termination_message( "Unrecoverable error" );
                            return 0;
                        }

                        for (int id = 0; id < mmol[p][m].size(); id++ )
                        {
                            ligandAtoms.push_back(mmol[p][m][id]);  // add the ligand atoms to a second array
                            allAtoms.push_back(mmol[p][m][id]);
                        }
                    }

                }

                else
                {
                    if ( strncmp( mmol[p][m].type().c_str(), input_ccd_code.trim().c_str(), 3 )) // true if strings are different
                    {
                        for (int id = 0; id < mmol[p][m].size(); id++ )
                        {
                            mainAtoms.push_back(mmol[p][m][id]); // cycle through atoms and copy them
                            allAtoms.push_back(mmol[p][m][id]);
                        }
                    }
                    else // it's the one sugar we're looking to omit
                    {
                        if ( input_validation_options.size() > 0 )
                        {
                            const clipper::MSugar msug ( mmol, mmol[p][m], manb, external_validation );

                            sugarList.push_back(mmol[p][m]);
                            ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (mmol[p].id().trim(), msug));
                        }
                        else
                        {
                            const clipper::MSugar msug(mmol, mmol[p][m], manb);

                            sugarList.push_back(mmol[p][m]);
                            ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (mmol[p].id().trim(), msug));
                        }

                        for (int id = 0; id < mmol[p][m].size(); id++ )
                        {
                            ligandAtoms.push_back(mmol[p][m][id]);  // add the ligand atoms to a second array
                            allAtoms.push_back(mmol[p][m][id]);
                        }
                    }
                }
            }
        }

        if (!batch) printf("\nPDB \t    Sugar   \t  Q  \t Phi  \tTheta \t   Detected type   \tCnf\t<Bfac>\tCtx\t Ok?");
        if (!batch && showGeom) printf("\tBond lengths, angles and torsions, reported clockwise with in-ring oxygen as first vertex");
        if (!batch) printf("\n----\t------------\t-----\t------\t------\t-------------------\t---\t------\t---\t-----");
        if (!batch && showGeom) printf("\t------------------------------------------------------------------------------------------------------------");
        if (!batch) printf("\n");

        for (int index = 0; index < ligandList.size(); index++)
        {
            float x, y, z, maxX, maxY, maxZ, minX, minY, minZ;
            x = y = z = 0.0;
            maxX = maxY = maxZ = -999999.0;
            minX = minY = minZ =  999999.0;

            for (int natom = 0; natom < sugarList[index].size(); natom++)
            {
                if(sugarList[index][natom].coord_orth().x() > maxX) maxX=sugarList[index][natom].coord_orth().x(); // calculation of the sugar centre
                if(sugarList[index][natom].coord_orth().y() > maxY) maxY=sugarList[index][natom].coord_orth().y();
                if(sugarList[index][natom].coord_orth().z() > maxZ) maxZ=sugarList[index][natom].coord_orth().z();
                if(sugarList[index][natom].coord_orth().x() < minX) minX=sugarList[index][natom].coord_orth().x();
                if(sugarList[index][natom].coord_orth().y() < minY) minY=sugarList[index][natom].coord_orth().y();
                if(sugarList[index][natom].coord_orth().z() < minZ) minZ=sugarList[index][natom].coord_orth().z();
            }

            x = minX + ((maxX - minX)/2);
            y = minY + ((maxY - minY)/2);
            z = minZ + ((maxZ - minZ)/2);

            if (batch)
            {
                fprintf(output, "%c%c%c%c\t%s-",input_model[1+pos_slash],input_model[2+pos_slash],input_model[3+pos_slash],input_model[4+pos_slash], ligandList[index].second.type().c_str());
                fprintf(output, "%s-%s   ", ligandList[index].first.c_str(), ligandList[index].second.id().trim().c_str());
            }
            else
            {
                printf("%c%c%c%c\t%s-",input_model[1+pos_slash],input_model[2+pos_slash],input_model[3+pos_slash],input_model[4+pos_slash], ligandList[index].second.type().c_str());
                std::cout << ligandList[index].first << "-" << ligandList[index].second.id().trim() << "  ";
            }

            if (batch)
            {
                std::vector<clipper::ftype> cpParams(10, 0);
                cpParams = ligandList[index].second.cremer_pople_params();
                fprintf(output,"\t%1.3f\t%3.2f\t",cpParams[0],cpParams[1]);  // output cremer-pople parameters
                if ( cpParams[2] == -1 ) fprintf ( output, " --  \t" ); else fprintf ( output, "%3.2f\t", cpParams[2] );
                fprintf(output,"%s\t", ligandList[index].second.type_of_sugar().c_str()); // output the type of sugar, e.g. alpha-D-aldopyranose
                fprintf(output,"%s\t", ligandList[index].second.conformation_name().c_str()); // output a 3 letter code for the conformation

                float bfac = 0.0;
                for (int i=0; i < ligandList[index].second.size(); i++)
                    bfac+=ligandList[index].second[i].u_iso();
                bfac /= ligandList[index].second.size();
                bfac  = clipper::Util::u2b(bfac);

                fprintf ( output, "%3.2f", bfac );     // output <bfactor>


                std::vector < clipper::MGlycan > list_of_glycans = mgl.get_list_of_glycans();
                bool found_in_tree = false;
                
                for ( int i = 0 ; i < list_of_glycans.size() ; i++ )
                {
                    std::vector < clipper::MSugar > list_of_sugars = list_of_glycans[i].get_sugars();

                    for ( int j = 0 ; j < list_of_sugars.size() ; j++ )
                    {
                        if ( list_of_sugars[j].id().trim() == ligandList[index].second.id().trim() )
                        {
                            if ( list_of_glycans[i].get_type() == "n-glycan" )
                            {
                                ligandList[index].second.set_context ( "n-glycan" );
                                fprintf ( output, "\t(n) " );
                            }
                            else if ( list_of_glycans[i].get_type() == "c-glycan" )
                            {
                                ligandList[index].second.set_context ( "c-glycan" );
                                fprintf ( output, "\t(c) " );
                            }
                            else if ( list_of_glycans[i].get_type() == "o-glycan" )
                            {
                                ligandList[index].second.set_context ( "o-glycan" );
                                fprintf ( output, "\t(o) " );
                            }
                            else if ( list_of_glycans[i].get_type() == "s-glycan" )
                            {
                                ligandList[index].second.set_context ( "s-glycan" );
                                fprintf ( output, "\t(s) " );
                            }
                            found_in_tree = true;
                            break;
                        }
                    }

                    if ( found_in_tree ) break;
                }

                if ( !found_in_tree )
                {
                    ligandList[index].second.set_context ( "ligand" );
                    fprintf ( output, "\t(l) ");
                }


                if (ligandList[index].second.in_database(ligandList[index].second.type().trim()))
                {
                    if ((ligandList[index].second.ring_members().size() == 6 ))
                    {
                        if (ligandList[index].second.is_sane())
                        {
                            if ( ! ligandList[index].second.ok_with_conformation() )
                            {
                                fprintf(output, "\tcheck");
                            }
                            else
                                fprintf(output, "\tyes");
                        }
                        else fprintf (output, "\tno");
                    }
                    else
                        if (ligandList[index].second.is_sane())
                            fprintf(output, "\tyes");
                        else
                        {
                            fprintf(output, "\tno");
                        }
                }
                else fprintf(output, "\tunk");

                if ( ! ligandList[index].second.ok_with_conformation () )
                    enable_torsions_for.push_back (ligandList[index].second.type().trim());

                bool occupancy_check = false;
                std::vector<clipper::MAtom> ringcomponents = ligandList[index].second.ring_members();

                for ( int i = 0 ; i < ringcomponents.size() ; i++ )
                {
                    if (privateer::util::get_altconformation(ringcomponents[i]) != ' ')
                        occupancy_check = true;
                }

                if (showGeom)
                {
                    std::vector<clipper::ftype> rangles = ligandList[index].second.ring_angles();
                    std::vector<clipper::ftype> rbonds  = ligandList[index].second.ring_bonds();
                    std::vector<clipper::ftype> rtorsions = ligandList[index].second.ring_torsions();

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        fprintf(output, "\t%1.2f", rbonds[i]);

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        fprintf(output, "\t%3.1f", rangles[i]);

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        fprintf(output, "\t%3.1f", rtorsions[i]);
                }

                if (occupancy_check)
                    fprintf(output, " (*)");

                fprintf(output, "\n");

            }
            else
            {
                std::vector<clipper::ftype> cpParams(10, 0);
                cpParams = ligandList[index].second.cremer_pople_params();
                printf("\t%1.3f\t%3.2f\t",cpParams[0],cpParams[1]);  // output cremer-pople parameters
                if ( cpParams[2] == -1 ) printf ( " --  \t" ); else printf ( "%3.2f\t", cpParams[2] );
                printf("%s\t", ligandList[index].second.type_of_sugar().c_str()); // output the type of sugar, e.g. alpha-D-aldopyranose
                printf("%s\t", ligandList[index].second.conformation_name().c_str()); // output a 3 letter code for the conformation

                float bfac = 0.0;

                for (int i=0; i < ligandList[index].second.size(); i++)
                    bfac+=ligandList[index].second[i].u_iso();

                bfac /= ligandList[index].second.size();
                bfac  = clipper::Util::u2b(bfac);
                printf ( "%3.2f", bfac ); // output <Bfactor>


                std::vector < clipper::MGlycan > list_of_glycans = mgl.get_list_of_glycans();
                bool found_in_tree = false;

                for ( int i = 0 ; i < list_of_glycans.size() ; i++ )
                {
                    std::vector < clipper::MSugar > list_of_sugars = list_of_glycans[i].get_sugars();
                    for ( int j = 0 ; j < list_of_sugars.size() ; j++ )
                    {
                        if ( list_of_sugars[j].id().trim() == ligandList[index].second.id().trim() )
                        {
                            if ( list_of_glycans[i].get_type() == "n-glycan" )
                            {
                                ligandList[index].second.set_context ( "n-glycan" );
                                std::cout << "\t(n) ";
                            }
                            else if ( list_of_glycans[i].get_type() == "c-glycan" )
                            {
                                ligandList[index].second.set_context ( "c-glycan" );
                                std::cout << "\t(c) ";
                            }
                            else if ( list_of_glycans[i].get_type() == "o-glycan" )
                            {
                                ligandList[index].second.set_context ( "o-glycan" );
                                std::cout << "\t(o) ";
                            }
                            else if ( list_of_glycans[i].get_type() == "s-glycan" )
                            {
                                ligandList[index].second.set_context ( "s-glycan" );
                                std::cout << "\t(s) ";
                            }
                            found_in_tree = true;
                            break;
                        }
                    }
                    if ( found_in_tree ) break;
                }

                if ( !found_in_tree )
                {
                    ligandList[index].second.set_context ( "ligand" );
                    std::cout << "\t(l) ";
                }


                if (ligandList[index].second.in_database(ligandList[index].second.type().trim()))
                {
                    if ((ligandList[index].second.ring_members().size() == 6 ))
                    {
                        if (ligandList[index].second.is_sane())
                        {
                            if ( ! ligandList[index].second.ok_with_conformation () )
                            {
                                printf("\tcheck");
                            }
                            else printf("\tyes");
                        }
                        else
                            printf ("\tno");
                    }
                    else
                        if (ligandList[index].second.is_sane())
                            printf("\tyes");
                        else
                        {
                            printf("\tno");
                        }
                }
                else printf("\tunk");

                if ( ! ligandList[index].second.ok_with_conformation () )
                    enable_torsions_for.push_back (ligandList[index].second.type().trim());
                    
                bool occupancy_check = false;
                std::vector<clipper::MAtom> ringcomponents = ligandList[index].second.ring_members();

                for ( int i = 0 ; i < ringcomponents.size() ; i++ )
                {
                    if (privateer::util::get_altconformation(ringcomponents[i]) != ' ')
                        occupancy_check = true;
                }

                if (showGeom)
                {
                    std::vector<clipper::ftype> rangles = ligandList[index].second.ring_angles();
                    std::vector<clipper::ftype> rbonds  = ligandList[index].second.ring_bonds();
                    std::vector<clipper::ftype> rtorsions = ligandList[index].second.ring_torsions();


                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        printf("\t%1.2f", rbonds[i]);
                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        printf("\t%3.1f", rangles[i]);
                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        printf("\t%3.1f", rtorsions[i]);
                }

                if (occupancy_check)
                    std::cout << " (*)";

                std::cout << std::endl;
            }
        }

        if (!batch)
        {
            std::cout << "\nPartially occupied monosaccharides, if any, are marked with an asterisk (*)";
            std::cout << std::endl << std::endl;
        }
        else
            fclose(output);

        std::fstream of_scm; std::fstream of_py;
        of_scm.open("privateer-results.scm", std::fstream::out);
        of_py.open("privateer-results.py", std::fstream::out);
        privateer::coot::insert_coot_prologue_scheme ( of_scm );
        privateer::coot::insert_coot_prologue_python ( of_py );

        clipper::String all_MapName, dif_MapName, omit_dif_MapName;
        all_MapName = ""; dif_MapName = ""; omit_dif_MapName = "";

        privateer::coot::insert_coot_files_loadup_scheme (of_scm, input_model, all_MapName, dif_MapName, omit_dif_MapName, batch, "input_model_nowater.pdb", check_unmodelled);
        privateer::coot::insert_coot_files_loadup_python (of_py , input_model, all_MapName, dif_MapName, omit_dif_MapName, batch, "input_model_nowater.pdb", check_unmodelled);





        int n_geom = 0, n_anomer = 0, n_config = 0, n_pucker = 0, n_conf = 0;

        int sugar_count = 0;

        for (int k = 0 ; k < ligandList.size() ; k++)
        {
            int n_errors = 0;

            clipper::String diagnostic = ligandList[k].second.type().trim() + "/" + ligandList[k].first + "/" + ligandList[k].second.id().trim() + ": " ;

            if (ligandList[k].second.is_supported() )
            {
                if ( ! ligandList[k].second.is_sane() )
                {
                    sugar_count++;

                    if (!ligandList[k].second.ok_with_anomer())
                    {
                        if ( n_errors > 0 )
                            diagnostic.append(", wrong anomer");
                        else
                            diagnostic.append("wrong anomer");

                        n_errors++; n_anomer++;
                    }

                    if (!ligandList[k].second.ok_with_chirality())
                    {
                        if ( n_errors > 0 )
                            diagnostic.append(", wrong configuration at " + ligandList[k].second.configurational_carbon().name().trim());
                        else
                            diagnostic.append("wrong configuration at " + ligandList[k].second.configurational_carbon().name().trim());

                        n_errors++; n_config++;
                    }

                    if (!ligandList[k].second.ok_with_puckering())
                    {
                        if ( n_errors > 0 )
                            diagnostic.append(", Q=" + clipper::String(ligandList[k].second.puckering_amplitude()) );
                        else
                            diagnostic.append("Q=" + clipper::String( ligandList[k].second.puckering_amplitude() ));

                        n_errors++; n_pucker++;
                    }

                    if (!ligandList[k].second.ok_with_conformation() )
                    {
                        if ( n_errors > 0 )
                            diagnostic.append(", conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken") );
                        else
                            diagnostic.append("conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken"));

                        n_errors++; n_conf++;
                    }

                    privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, ligandList[k].second.ring_centre(), diagnostic );
                    privateer::coot::insert_coot_go_to_sugar_python ( of_py, ligandList[k].second.ring_centre(), diagnostic );
                }
                else // sugar is sane, but still need to check higher-energy conformations
                {
                    if ( !ligandList[k].second.ok_with_conformation() )
                    {
                        if ( n_errors > 0 )
                            diagnostic.append(", conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken") );
                        else
                            diagnostic.append("conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken"));

                        n_errors++;
                        n_conf++;
                        sugar_count++;

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, ligandList[k].second.ring_centre(), diagnostic );
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, ligandList[k].second.ring_centre(), diagnostic );
                    }
                }
            }
        }

        clipper::String status_msg = "Blue map: 2mFo-DFc. Pink map: omit mFo-DFc. Torsion restraints have been enabled.";

        privateer::coot::insert_coot_epilogue_scheme ( of_scm );
        privateer::coot::insert_coot_epilogue_python ( of_py );

        privateer::coot::insert_coot_statusbar_text_scheme ( of_scm, status_msg );
        privateer::coot::insert_coot_statusbar_text_python ( of_py, status_msg );

        of_scm.close();
        of_py.close();

        std::cout << "SUMMARY: " << std::endl << std::endl ;
        std::cout << "   Wrong anomer: " << n_anomer << std::endl;
        std::cout << "   Wrong configuration: " << n_config << std::endl;
        std::cout << "   Unphysical puckering amplitude: " << n_pucker << std::endl;
        std::cout << "   In higher-energy conformations: " << n_conf << std::endl;
        std::cout << std::endl;
        std::cout << "   Privateer has identified " << n_anomer + n_config + n_pucker + n_conf;
        std::cout << " issues, with " << sugar_count << " of " << ligandList.size() << " sugars affected." << std::endl;

        if ( enable_torsions_for.size() > 0 )
        {
            privateer::util::write_refmac_keywords ( enable_torsions_for );
            privateer::util::write_libraries( enable_torsions_for );
        }

        prog.set_termination_message( "Normal termination" );
        system("touch scored");
        return 0;

    }


    // Full analysis, slower but much more complete //

    std::vector< std::string > enable_torsions_for;

    if      (useMTZ && !useMRC) privateer::xray::read_xray_map( input_reflections_mtz, input_model, mmol, hklinfo, mtzin );
    else if (useMRC && !useMTZ)
    {
        privateer::cryo_em::read_cryoem_map ( input_cryoem_map, hklinfo, cryo_em_map, mrcin, resolution);
        privateer::util::read_coordinate_file_mrc (mfile, mmol, input_model, cryo_em_map, batch);
    }
    else 
    {
        // assume CIF file format instead
        if (!batch)
            std::cout << "Opening CIF file: " << input_reflections_cif.trim() << std::endl;

        cifin.open_read( input_reflections_cif.trim().c_str() );

        try // we could be in trouble should the CIF file not have cell parameters
        {
            cifin.import_hkl_info( hklinfo );     // read spacegroup, cell, resolution, HKL's
        }
        catch (...)
        {
            if (!batch) std::cout << "\nReading cell and spacegroup parameters from the CRYST1 card in " << input_model;
            std::cout << ":\n Spacegroup (" << mmol.spacegroup().spacegroup_number() << ")\n" << mmol.cell().format() << "\n\n";

            clipper::Resolution myRes(0.96);
            hklinfo = clipper::HKL_info( mmol.spacegroup(), mmol.cell(), myRes, true);
        }
    }

    clipper::HKL_data<clipper::data32::F_sigF> fobs;            // allocate space for F and sigF
    clipper::HKL_data<clipper::data32::F_sigF> fobs_scaled;     // allocate space for scaled F and sigF
    clipper::HKL_data<clipper::data32::F_phi> fc_omit_bsc;      // allocate space for the omit calc data with bsc
    clipper::HKL_data<clipper::data32::F_phi> fc_all_bsc;       // allocate space for the whole calculated data with bsc
    clipper::HKL_data<clipper::data32::F_phi> fc_ligands_bsc;    // allocate space for the ligand calculated data


    clipper::HKL_data<clipper::data32::F_phi> fc_cryoem_obs;    // allocate space for cryoEM calculated structure factors, that acts as observed data.
    clipper::HKL_data<clipper::data32::F_phi> fc_all_cryoem_data; // allocate space for entire cryoEM calculated model, that acts as calculated data.
    clipper::HKL_data<clipper::data32::F_phi> fc_ligands_only_cryoem_data; // allocate space for calculated cryoEM model of ligands only, that acts as calculated data.
    

    if (!useMTZ && !useMRC && !noMaps)
    {
        try
        {
            fobs = clipper::HKL_data<clipper::data32::F_sigF> ( hklinfo );
            fobs_scaled = clipper::HKL_data<clipper::data32::F_sigF> ( hklinfo );
            fc_omit_bsc = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );
            fc_all_bsc = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );
            fc_ligands_bsc = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );

            cifin.import_hkl_data( fobs );
            cifin.close_read();
        }
        catch (...)
        {
            if (!batch) std::cout << "\nNo suitable amplitudes found in the CIF file!\n\nExiting...\n\n";
            return 1;
        }
    }
    else
    {
        if (useMTZ)
        {
            fobs = clipper::HKL_data<clipper::data32::F_sigF> ( hklinfo );
            fobs_scaled = clipper::HKL_data<clipper::data32::F_sigF> ( hklinfo );
            fc_omit_bsc = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );
            fc_all_bsc = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );
            fc_ligands_bsc = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );

            privateer::xray::initialize_experimental_dataset( mtzin, ampmtzin, input_column_fobs, fobs, hklinfo, opxtal, opdset, input_reflections_mtz);
            std::cout << std::endl << " " << fobs.num_obs() << " reflections have been loaded";
            std::cout << std::endl << std::endl << " Resolution " << hklinfo.resolution().limit() << "Å" << std::endl << hklinfo.cell().format() << std::endl;
            fobs_scaled = fobs;
        }
        if (useMRC)
        {
            // calculate cryo em map structure factors;
            fobs = clipper::HKL_data<clipper::data32::F_sigF> ( hklinfo );
            fc_cryoem_obs = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo, cryo_em_map.cell() );
            fc_all_cryoem_data = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );
            fc_ligands_only_cryoem_data = clipper::HKL_data<clipper::data32::F_phi> ( hklinfo );
         
            cryo_em_map.fft_to(fc_cryoem_obs);
            // privateer::cryo_em::initialize_dummy_fobs( fobs, fc_cryoem_obs ); // might not be necessary at all.
        }
    }


   
    clipper::Atom_list mainAtoms;
    clipper::Atom_list ligandAtoms;
    clipper::Atom_list allAtoms;

    std::vector<std::pair< clipper::String , clipper::MSugar> > ligandList; // we store the Chain ID and create an MSugar to be scored
    std::vector<clipper::MMonomer> sugarList; // store the original MMonomer

    const clipper::MAtomNonBond& manb = clipper::MAtomNonBond( mmol, 1.0 ); // was 1.0

    mgl = clipper::MGlycology(mmol, manb, input_expression_system);

    list_of_glycans = mgl.get_list_of_glycans();
    list_of_glycans_associated_to_permutations.resize(list_of_glycans.size());

    // expand the alternativeGlycans big vector here to list of glycans and match indices. so like original glycan -> all of its permutations + scores and so on.
    //                                                                                             original glycan -> all of its permutations 
    // std::vector<std::vector<std::pair<clipper::MGlycan, std::vector<int>>>> originalGlycansAndPermutations(list_of_glycans.size());
    if ( !batch )
    {
        std::cout << std::endl << "Number of detected glycosylations: " << list_of_glycans.size();

        if ( list_of_glycans.size() > 0 )
        {
            int glycansPermutated = 0;
            clipper::String current_chain = "" ;

            for (int i = 0; i < list_of_glycans.size() ; i++ )
            {  
                clipper::String wurcs_string;
                if ( current_chain != list_of_glycans[i].get_chain() )
                {
                    current_chain = list_of_glycans[i].get_chain();
                    std::cout << std::endl << std::endl << "Chain " << current_chain[0] << std::endl << "-------" << std::endl ;
                }
                std::cout << std::endl << list_of_glycans[i].print_linear ( true, false, true ) << std::endl;

                wurcs_string = list_of_glycans[i].generate_wurcs();
                std::cout << wurcs_string << std::endl;

                if(useWURCSDataBase)
                {
                    std::vector<std::pair<std::pair<clipper::MGlycan, std::vector<int>>,float>> finalGlycanPermutationContainer;
                    output_dbquery(jsonObject, wurcs_string, list_of_glycans[i], finalGlycanPermutationContainer, glucose_only, clean_output, sleepTimer, pool, useParallelism);

                    if (useParallelism)
                    {
                        #if DUMP
                            std::cout << std::endl;
                            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
                        #endif
                        
                        while(pool.n_remaining_jobs() > 0)
                            pool.sync();
                        
                        #if DUMP
                            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
                        #endif
                    }
                    
                    if(!finalGlycanPermutationContainer.empty())
                        {
                            glycansPermutated++;
                            list_of_glycans_associated_to_permutations.at(i) = finalGlycanPermutationContainer;
                            for(int j = 0; j < finalGlycanPermutationContainer.size(); j++)
                                {
                                    if(oldstyleinput)
                                    {
                                        privateer::glycoplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                                        plot.plot_glycan ( finalGlycanPermutationContainer[j].first.first );
                                        std::ostringstream os;
                                        os << finalGlycanPermutationContainer[j].first.first.get_root_for_filename() << "-" << j << "-PERMUTATION.svg";
                                        plot.write_to_file ( os.str() );
                                    }
                                    else 
                                    {
                                        privateer::glycanbuilderplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                                        plot.plot_glycan ( finalGlycanPermutationContainer[j].first.first );
                                        std::ostringstream os;
                                        os << finalGlycanPermutationContainer[j].first.first.get_root_for_filename() << "-" << j << "-PERMUTATION.svg";
                                        plot.write_to_file ( os.str() );
                                    }
                                }
                        }
                }
                if(oldstyleinput)
                {
                    privateer::glycoplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                    plot.plot_glycan ( list_of_glycans[i] );
                    std::ostringstream os;
                    os << list_of_glycans[i].get_root_for_filename() << ".svg";
                    plot.write_to_file ( os.str() );
                }
                else
                {
                    privateer::glycanbuilderplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                    plot.plot_glycan ( list_of_glycans[i] );
                    std::ostringstream os;
                    os << list_of_glycans[i].get_root_for_filename() << ".svg";
                    plot.write_to_file ( os.str() );
                }
            }
            
            if(useWURCSDataBase && glycansPermutated > 0) std::cout << "Originally modelled glycans not found on GlyConnect database: " << glycansPermutated << "/" << list_of_glycans.size() << std::endl;
        }

        std::cout << std::endl << "Analysing carbohydrates... ";
        fflush(0);
    }
    else
    {
        for (int i = 0; i < list_of_glycans.size() ; i++ )
        {
            
            clipper::String wurcs_string;
            int glycansPermutated = 0;

            wurcs_string = list_of_glycans[i].generate_wurcs();
            std::cout << wurcs_string << std::endl;

            if(useWURCSDataBase)
            {
                std::vector<std::pair<std::pair<clipper::MGlycan, std::vector<int>>,float>> finalGlycanPermutationContainer;
                output_dbquery(jsonObject, wurcs_string, list_of_glycans[i], finalGlycanPermutationContainer, glucose_only, clean_output, sleepTimer, pool, useParallelism);

                if (useParallelism)
                {
                    #if DUMP
                        std::cout << std::endl;
                        DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
                    #endif
                    
                    while(pool.n_remaining_jobs() > 0)
                        pool.sync();
                    
                    #if DUMP
                        DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
                    #endif
                }
                
                if(!finalGlycanPermutationContainer.empty())
                    {
                        glycansPermutated++;
                        list_of_glycans_associated_to_permutations.at(i) = finalGlycanPermutationContainer;
                        for(int j = 0; j < finalGlycanPermutationContainer.size(); j++)
                            {
                                    if(oldstyleinput)
                                    {
                                        privateer::glycoplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                                        plot.plot_glycan ( finalGlycanPermutationContainer[j].first.first );
                                        std::ostringstream os;
                                        os << finalGlycanPermutationContainer[j].first.first.get_root_for_filename() << "-" << j << "-PERMUTATION.svg";
                                        plot.write_to_file ( os.str() );
                                    }
                                    else 
                                    {
                                        privateer::glycanbuilderplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                                        plot.plot_glycan ( finalGlycanPermutationContainer[j].first.first );
                                        std::ostringstream os;
                                        os << finalGlycanPermutationContainer[j].first.first.get_root_for_filename() << "-" << j << "-PERMUTATION.svg";
                                        plot.write_to_file ( os.str() );
                                    }
                            }
                    }
            }
            if(oldstyleinput)
            {
                privateer::glycoplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                plot.plot_glycan ( list_of_glycans[i] );
                std::ostringstream os;
                os << list_of_glycans[i].get_root_for_filename() << ".svg";
                plot.write_to_file ( os.str() );
            }
            else
            {
                privateer::glycanbuilderplot::Plot plot(vertical, original, list_of_glycans[i].get_root_by_name(), invert, true);
                plot.plot_glycan ( list_of_glycans[i] );
                std::ostringstream os;
                os << list_of_glycans[i].get_root_for_filename() << ".svg";
                plot.write_to_file ( os.str() );
            }
        }

    }

    std::vector<std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > > blobsProteinBackboneSummaryForCoot(6);
    if ( check_unmodelled )
    {
        std::cout << std::endl << "___________________________________________________________________" << std::endl;
        std::cout << "Scanning a waterless difference map for unmodelled glycosylation sites on protein backbone..." << std::endl;

        clipper::MiniMol modelRemovedWaters = get_model_without_waters(input_model);

        clipper::Atom_list withoutWaterModelAtomList = modelRemovedWaters.atom_list();
        
        std::vector<std::vector<GlycosylationMonomerMatch> > PotentialMonomers = get_matching_monomer_positions(modelRemovedWaters);



        clipper::Grid_sampling mygrid( hklinfo.spacegroup(), hklinfo.cell(), hklinfo.resolution() );
        // clipper::Grid_sampling mygrid( cryo_em_map.grid_asu().nu(), cryo_em_map.grid_asu().nv(), cryo_em_map.grid_asu().nw() );

        clipper::Xmap<float> sigmaa_all_map( hklinfo.spacegroup(), hklinfo.cell(), mygrid );
        clipper::Xmap<float> sigmaa_dif_map( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

        std::cout << "Imports were successful." << std::endl;

        bool no_errors = false; 
        if (noMaps && !useMTZ && !useMRC)
        {
            std::cout << std::endl << "Error: this feature requires a map input. Please either import cryo-em map(.map) or x-ray crystallography structure factors(.mtz)" << std::endl;
            prog.set_termination_message( "Failed" );
            return 1;
        }
        else
        {
            if (useMTZ) 
            {
                if (useMRC)
                {
                    std::cout << std::endl << "Error: Expected to only have a single map input, not both initialized MTZ and MTZ objects. Aborting." << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
                else 
                    no_errors = privateer::util::calculate_sigmaa_maps ( withoutWaterModelAtomList, fobs, fc_cryoem_obs, sigmaa_all_map, sigmaa_dif_map, ignore_set_null, useMTZ );
            }
            else 
            {
                if (useMTZ)
                {
                    std::cout << std::endl << "Error: Expected to only have a single map input, not both initialized MTZ and MTZ objects. Aborting." << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                }
                else 
                    std::cout << std::endl << "Error: This function is currently unsupported for cryo em maps." << std::endl;
                    prog.set_termination_message( "Failed" );
                    return 1;
                    // no_errors = privateer::util::calculate_sigmaa_maps ( withoutWaterModelAtomList, fobs, fc_cryoem_obs, sigmaa_all_map, sigmaa_dif_map, ignore_set_null, useMTZ);                                             
            }

        }
        clipper::Map_stats ms(sigmaa_dif_map);
	    float map_sigma = ms.std_dev();
        std::cout << "Status of no_errors " << std::boolalpha << no_errors << std::endl;
        if (no_errors)
            {
                std::cout << std::endl << "Sigmaa difference map was successfully generated: " << std::boolalpha << no_errors << std::endl;
                std::cout << std::endl;
                std::stringstream buffer;

                for(int type = 0; type < 5; type++)
                {
                    std::vector<std::pair<PotentialGlycosylationSiteInfo, double> > results;
                    results = get_electron_density_of_potential_glycosylation_sites(PotentialMonomers, type, modelRemovedWaters, sigmaa_dif_map, hklinfo, list_of_glycans, ms, thresholdElectronDensityValue, check_unmodelled);


                    if(!results.empty())
                    {
                        if(type == 0)
                        {
                        std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > N_SiteBlobs;
                            for (int i = 0; i < results.size(); i++)
                            {

                                buffer << "\tN-Glycosylation: Value of experimental mean electron density in detected consensus sequence for" << mmol[results[i].first.chainID][results[i].first.monomerID].id() <<
                                "-" << mmol[results[i].first.chainID][results[i].first.monomerID].type()
                                << " monomer in Chain " << mmol[results[i].first.chainID].id() << ": " << results[i].second << std::endl;

                                std::tuple <clipper::String, clipper::MMonomer, double> blobInfo(modelRemovedWaters[results[i].first.chainID].id(), modelRemovedWaters[results[i].first.chainID][results[i].first.monomerID], results[i].second);
                                N_SiteBlobs.push_back(blobInfo);

                            }
                        blobsProteinBackboneSummaryForCoot.at(type) = N_SiteBlobs;
                        buffer << std::endl;
                        }
                        if(type == 1)
                        {
                        std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > C_SiteBlobs;
                            for (int i = 0; i < results.size(); i++)
                            {
                                buffer << "\tC-Glycosylation: Value of experimental mean electron density in detected consensus sequence for" << mmol[results[i].first.chainID][results[i].first.monomerID].id() <<
                                "-" << mmol[results[i].first.chainID][results[i].first.monomerID].type()
                                << " monomer in Chain " << mmol[results[i].first.chainID].id() << ": " << results[i].second << std::endl;
                            
                                std::tuple <clipper::String, clipper::MMonomer, double> blobInfo(modelRemovedWaters[results[i].first.chainID].id(), modelRemovedWaters[results[i].first.chainID][results[i].first.monomerID], results[i].second);
                                C_SiteBlobs.push_back(blobInfo);
                            }
                        blobsProteinBackboneSummaryForCoot.at(type) = C_SiteBlobs;
                        buffer << std::endl;
                        }
                        if(type == 2)
                        {
                        std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > O_SiteBlobs;
                            for (int i = 0; i < results.size(); i++)
                            {
                                buffer << "\tO-Glycosylation: Value of experimental mean electron density in detected consensus sequence for" << mmol[results[i].first.chainID][results[i].first.monomerID].id() <<
                                "-" << mmol[results[i].first.chainID][results[i].first.monomerID].type()
                                << " monomer in Chain " << mmol[results[i].first.chainID].id() << ": " << results[i].second << std::endl;

                                std::tuple <clipper::String, clipper::MMonomer, double> blobInfo(modelRemovedWaters[results[i].first.chainID].id(), modelRemovedWaters[results[i].first.chainID][results[i].first.monomerID], results[i].second);
                                O_SiteBlobs.push_back(blobInfo);                            
                            }
                        blobsProteinBackboneSummaryForCoot.at(type) = O_SiteBlobs;
                        buffer << std::endl;
                        }

                        if(type == 3)
                        {
                        std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > S_SiteBlobs;
                            for (int i = 0; i < results.size(); i++)
                            {
                                buffer << "\tS-Glycosylation: Value of experimental mean electron density in detected consensus sequence for" << mmol[results[i].first.chainID][results[i].first.monomerID].id() <<
                                "-" << mmol[results[i].first.chainID][results[i].first.monomerID].type()
                                << " monomer in Chain " << mmol[results[i].first.chainID].id() << ": " << results[i].second << std::endl;

                                std::tuple <clipper::String, clipper::MMonomer, double> blobInfo(modelRemovedWaters[results[i].first.chainID].id(), modelRemovedWaters[results[i].first.chainID][results[i].first.monomerID], results[i].second);
                                S_SiteBlobs.push_back(blobInfo);                          
                            }
                        blobsProteinBackboneSummaryForCoot.at(type) = S_SiteBlobs;
                        buffer << std::endl;
                        }
                        
                        if(type == 4)
                        {
                        std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > NRem_SiteBlobs;
                            for (int i = 0; i < results.size(); i++)
                            {
                                buffer << "\tPossibly processed by PNGase F: Value of experimental mean electron density in detected consensus sequence for" << mmol[results[i].first.chainID][results[i].first.monomerID].id() <<
                                "-" << mmol[results[i].first.chainID][results[i].first.monomerID].type()
                                << " monomer in Chain " << mmol[results[i].first.chainID].id() << ": " << results[i].second << std::endl;

                                std::tuple <clipper::String, clipper::MMonomer, double> blobInfo(modelRemovedWaters[results[i].first.chainID].id(), modelRemovedWaters[results[i].first.chainID][results[i].first.monomerID], results[i].second);
                                NRem_SiteBlobs.push_back(blobInfo);                            
                            }
                        blobsProteinBackboneSummaryForCoot.at(type) = NRem_SiteBlobs;
                        buffer << std::endl;
                        }
                    }
                }

                if (!buffer.str().empty())
                {
                    std::cout << "Detected possibly unmodelled Glycosylation sites on these protein backbones:" << std::endl;
                    std::cout << buffer.str() << std::endl;
                }
                else
                {
                    std::cout << "\tPossibly unmodelled Glycosylation was not detected in this model." << std::endl;
                }
            }

            std::cout << "Finished scanning waterless difference map for unmodelled glycosylation sites on protein backbone..." << std::endl;
            

            if(!list_of_glycans.empty())
            {
            int type = 5;
            std::vector< std::tuple <clipper::String, clipper::MMonomer, double> > MIA_CarbsBlobs;
            std::stringstream buffer;
            std::cout << std::endl << "Scanning for unmodelled glycan monomers at modelled glycan chains. " << std::endl;
            for (int id = 0; id < list_of_glycans.size(); id++ )
                {
                    std::vector<std::pair<GlycanToMiniMolIDs, double> > densityInfo;
                    std::vector < clipper::MSugar > glycanChain;
                    glycanChain = list_of_glycans[id].get_sugars();
                    densityInfo = get_electron_density_of_potential_unmodelled_carbohydrate_monomers(glycanChain, modelRemovedWaters, list_of_glycans, id, sigmaa_dif_map, hklinfo, ms, thresholdElectronDensityValue, check_unmodelled);
                    if (!densityInfo.empty())
                    {
                        for(int i = 0; i < densityInfo.size(); i++)
                        {
                            int sugarID = densityInfo[i].first.carbohydrateID;
                            double meanElectronDensity = densityInfo[i].second;
                            buffer << "\tPossibly unmodelled carbohydrate in Chain " << list_of_glycans[id].get_chain()[0] << " of " << glycanChain[sugarID].id() << "-" << glycanChain[sugarID].type() << " - mean ED Value: " << meanElectronDensity << std::endl;
                            std::tuple <clipper::String, clipper::MMonomer, double> blobInfo(modelRemovedWaters[densityInfo[i].first.proteinMiniMolID].id(), modelRemovedWaters[densityInfo[i].first.proteinMiniMolID][densityInfo[i].first.carbohydrateChainMiniMolID], meanElectronDensity);
                            MIA_CarbsBlobs.push_back(blobInfo);
                        }
                        buffer << std::endl;
                    }
                }
                blobsProteinBackboneSummaryForCoot.at(type) = MIA_CarbsBlobs;

            if (!buffer.str().empty())
                {
                    std::cout << std::endl << "Detected possibly unmodelled Carbohydrate monomers in these Glycans:" << std::endl;
                    std::cout << std::endl << buffer.str() << std::endl;
                }
                else
                {
                    std::cout << std::endl << "\tPossibly unmodelled Carbohydrate monomers were not detected in this model." << std::endl;
                }    
            }


            std::cout << "___________________________________________________________________" << std::endl;

            if(check_unmodelled)
            {
                for ( int chn = 0; chn < modelRemovedWaters.size(); chn++ ) 
                {
                    if ( modelRemovedWaters[chn].id() == "" ) 
                    {
                        clipper::String label = mmol[chn].id();
                        modelRemovedWaters[chn].set_id(label);
                    }
                }
                 
                clipper::MMDBfile pdbfile;
                pdbfile.export_minimol( modelRemovedWaters );
                pdbfile.write_file( "input_model_nowater.pdb" );

                clipper::CCP4MAPfile sigmaa_dif_nowater;
                sigmaa_dif_nowater.open_write( "sigmaa_diff_nowater.map" );
                sigmaa_dif_nowater.export_xmap( sigmaa_dif_map );
                sigmaa_dif_nowater.close_write();

                std::cout << "Finished outputting sigmaa_diff_nowater.map!" << std::endl;
            }
    }

    // erase ligand atoms from the model and then calculate phases

    for ( int p = 0; p < mmol.size(); p++ )
    {
        for ( int m = 0; m < mmol[p].size(); m++ )
        {
            if (allSugars)
            {
                if ( clipper::MDisaccharide::search_disaccharides(mmol[p][m].type().c_str()) != -1 ) // treat disaccharide
                {
                    clipper::MDisaccharide md(mmol, manb, mmol[p][m] );
                    sugarList.push_back ( mmol[p][m] );
                    sugarList.push_back ( mmol[p][m] );
                    clipper::String id = mmol[p].id();
                    id.resize(1);

                    ligandList.push_back ( std::pair < clipper::String, clipper::MSugar> (id, md.get_first_sugar()));
                    ligandList.push_back ( std::pair < clipper::String, clipper::MSugar> (id, md.get_second_sugar()));

                    if (( md.get_first_sugar().type_of_sugar() == "unsupported" ) || ( md.get_second_sugar().type_of_sugar() == "unsupported" ) )
                    {
                        std::cout << std::endl;
                        std::cout << "Error: strangely, at least one of the sugars in the supplied PDB file is missing required atoms. Stopping..." << std::endl << std::endl;
                        prog.set_termination_message( "Unrecoverable error" );
                        return 0;
                    }

                    for (int id = 0; id < mmol[p][m].size(); id++ )
                    {
                        ligandAtoms.push_back(mmol[p][m][id]);  // add the ligand atoms to a second array
                        allAtoms.push_back(mmol[p][m][id]);
                    }

                }
                else if ( !clipper::MSugar::search_database(mmol[p][m].type().c_str()) )
                {
                    for (int id = 0; id < mmol[p][m].size(); id++ )
                    {
                        mainAtoms.push_back(mmol[p][m][id]); // cycle through atoms and copy them
                        allAtoms.push_back(mmol[p][m][id]);
                    }
                }
                else // it's one of the sugars contained in the database
                {
                    clipper::MSugar msug, msug_b;

                    std::vector <char> conformers = privateer::util::number_of_conformers(mmol[p][m]);

                    // #if DUMP
                    //     std::cout << "number of alternate conformations: " << conformers.size() << std::endl;
                    // #endif

                    int n_conf = conformers.size();

                    if ( n_conf > 0 )
                    {
                        if ( n_conf == 1 )
                            msug   = clipper::MSugar(mmol, mmol[p][m], manb, conformers[0]);
                        else
                        {
                            msug   = clipper::MSugar(mmol, mmol[p][m], manb, conformers[0]);
                            msug_b = clipper::MSugar(mmol, mmol[p][m], manb, conformers[1]);
                        }

                    }
                    else
                    {
                        msug = clipper::MSugar(mmol, mmol[p][m], manb);
                    }

                    sugarList.push_back(mmol[p][m]);
                    clipper::String id = mmol[p].id();
                    id.resize(1);

                    ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (id, msug));
                    // add both conformers if the current monomer contains more than one
                    if ( n_conf == 2 )
                        ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (id, msug_b));

                    if ( msug.type_of_sugar() == "unsupported" )
                    {
                        std::cout << std::endl << "Error: at least one of the sugars in the supplied PDB file is missing expected atoms"
                                  << std::endl << std::endl;
                        prog.set_termination_message( "Unrecoverable error" );
                        return 0;
                    }

                    for (int id = 0; id < mmol[p][m].size(); id++ )
                    {
                        ligandAtoms.push_back(mmol[p][m][id]);  // add the ligand atoms to a second array
                        allAtoms.push_back(mmol[p][m][id]);
                    }
                }

            }

            else
            {
                if ( strncmp( mmol[p][m].type().c_str(), input_ccd_code.trim().c_str(), 3 )) // true if strings are different
                {
                    for (int id = 0; id < mmol[p][m].size(); id++ )
                    {
                        mainAtoms.push_back(mmol[p][m][id]); // cycle through atoms and copy them
                        allAtoms.push_back(mmol[p][m][id]);
                    }
                }
                else // it's the one sugar we're looking to omit
                {
                    if ( input_validation_options.size() > 0 )
                    {
                        const clipper::MSugar msug ( mmol, mmol[p][m], manb, external_validation );

                        sugarList.push_back(mmol[p][m]);
                        ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (mmol[p].id().trim(), msug));
                    }
                    else
                    {
                        const clipper::MSugar msug(mmol, mmol[p][m], manb);

                        sugarList.push_back(mmol[p][m]);
                        ligandList.push_back(std::pair<clipper::String, clipper::MSugar> (mmol[p].id().trim(), msug));

                    }
                    for (int id = 0; id < mmol[p][m].size(); id++ )
                    {
                        ligandAtoms.push_back(mmol[p][m][id]);  // add the ligand atoms to a second array
                        allAtoms.push_back(mmol[p][m][id]);
                    }
                }
            }
        }
    }


    if (useMRC && !useMTZ && !noMaps) //cryoem here
    {
        if (!batch) std::cout << "Done analyzing modelled carbohydrates.\nCalculating simulated structure factors from model input... "; fflush(0);
        
        privateer::cryo_em::calculate_sfcs_of_fc_maps ( fc_all_cryoem_data, fc_ligands_only_cryoem_data, allAtoms, ligandAtoms, pool, useParallelism);

        std::cout << "done." << std::endl << "Computing Fo-DFc map... ";
        fflush(0);


        clipper::Grid_sampling mygrid( cryo_em_map.grid_asu().nu(), cryo_em_map.grid_asu().nv(), cryo_em_map.grid_asu().nw() );
        clipper::Xmap<double> cryo_em_dif_map_all( hklinfo.spacegroup(), hklinfo.cell(), mygrid );          // define sigmaa diff  map
        clipper::Xmap<double> cryo_em_twotimes_obs_dif_map_all( hklinfo.spacegroup(), hklinfo.cell(), mygrid );          // define sigmaa diff  map
        clipper::Xmap<double> ligandmap( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

        // scale data and flag R-free

        HRI ih;

        clipper::HKL_data<F_phi> difference_coefficients( hklinfo );

        bool difference_map_sfc_generated = privateer::cryo_em::generate_output_map_coefficients(difference_coefficients, fc_cryoem_obs, fc_all_cryoem_data, hklinfo);

        if (!difference_map_sfc_generated)
            {
                std::cout << "\n\nUnable to calculate Fo-DFc structure factors for cryoEM map input. Exiting..." << std::endl << std::endl;
                prog.set_termination_message( "Failed" );
                return 1;
            }
    
        clipper::Xmap<double> modelmap( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

        if(useParallelism)
        {
            pool.push([&cryo_em_dif_map_all, &difference_coefficients](int id)
            { 
                #if DUMP
                    std::cout << std::endl;
                    DBG << "Calculating cryo_em_dif_map_all from Thread ID: " << id << '.' << std::endl;
                #endif
                
                cryo_em_dif_map_all.fft_from(difference_coefficients);
            });

            pool.push([&modelmap, &fc_all_cryoem_data](int id)
            { 
                #if DUMP
                    DBG << "Calculating modelmap from Thread ID: " << id << '.' << std::endl;
                #endif
                
                modelmap.fft_from(fc_all_cryoem_data);
            });

            pool.push([&ligandmap, &fc_ligands_only_cryoem_data](int id)
            { 
                #if DUMP
                    DBG << "Calculating ligandmap from Thread ID: " << id << '.' << std::endl;
                #endif
                
                ligandmap.fft_from(fc_ligands_only_cryoem_data);
            });
        }
        else
        {
            cryo_em_dif_map_all.fft_from( difference_coefficients );

            modelmap.fft_from( fc_all_cryoem_data ); 

            ligandmap.fft_from( fc_ligands_only_cryoem_data );       // this is the map that will serve as Fc map for the RSCC calculation
        }
        

        if (useParallelism)
        {
            #if DUMP
                std::cout << std::endl;
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
            #endif
            
            while(pool.n_remaining_jobs() > 0)
                pool.sync();
            
            #if DUMP
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
            #endif
        }


        if (!batch)
            std::cout << "done." << std::endl;


        if (!batch)
        {
            std::cout << "\nWriting maps to disk... ";
            fflush(0);
        }
        clipper::CCP4MAPfile diff_mapOut;
        clipper::CCP4MAPfile modelmapout; // remove later

        if (allSugars)
            input_ccd_code = "all";

        if (!batch)
        {
            
            // TO DO: Write a function in parallelism code that would reserve threads(or detach) that are used in this task and therefore would not require to be synced in subsequent steps.
            //        Might be more difficult to write than it sounds...
            if(useParallelism)
            {
                pool.push([&diff_mapOut, &cryo_em_dif_map_all](int id)
                { 
                    #if DUMP
                        std::cout << std::endl;
                        DBG << "Writing and outputting cryo_em_dif_map_all to \"cryoem_diff.map\" from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    diff_mapOut.open_write( "cryoem_diff.map" );
                    diff_mapOut.export_xmap( cryo_em_dif_map_all );
                    diff_mapOut.close_write();
                });

                pool.push([&modelmapout, &modelmap](int id)
                { 
                    #if DUMP
                        DBG << "Writing and outputting modelmap to \"cryoem_calcmodel.map\" from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    modelmapout.open_write( "cryoem_calcmodel.map" );
                    modelmapout.export_xmap( modelmap );
                    modelmapout.close_write();
                });
            }
            else
            {
                diff_mapOut.open_write( "cryoem_diff.map" );
                diff_mapOut.export_xmap( cryo_em_dif_map_all );
                diff_mapOut.close_write();

                modelmapout.open_write( "cryoem_calcmodel.map" );
                modelmapout.export_xmap( modelmap );
                modelmapout.close_write();

                std::cout << "done." << std::endl;
            }
            

            // this one does not need syncing all the way till the end, can just keep running on the side, while the rest executes. 

           
            std::cout << "\n\nDetailed validation data" << std::endl;
            std::cout << "------------------------" << std::endl;
        }
        
        if(useParallelism)
        {
            int processedMonomers = 0;
            for (int index = 0; index < ligandList.size(); index++)
            {
                if(pool.n_remaining_jobs() >= (pool.n_idle() - 1))
                    pool.sync();
                
                pool.push([&sugarList, &output, &input_model, &ligandList, &hklinfo, &mygrid, &cryo_em_map, &ligandmap, &mgl, &enable_torsions_for, showGeom, ipradius, pos_slash, index, batch](int id)
                { 
                    #if DUMP
                        std::cout << std::endl;
                        DBG << "Calculating RSCC score from Thread ID: " << id << " for nth " << index << " index out of " << ligandList.size() << " total indices." << std::endl;
                    #endif
                    
                    float x,y,z,maxX,maxY,maxZ,minX,minY,minZ;
                    x=y=z=0.0;
                    maxX=maxY=maxZ=-999999.0;
                    minX=minY=minZ=999999.0;

                    for (int natom = 0; natom < sugarList[index].size(); natom++)
                    {
                        if(sugarList[index][natom].coord_orth().x() > maxX) maxX=sugarList[index][natom].coord_orth().x(); // calculation of the sugar centre
                        if(sugarList[index][natom].coord_orth().y() > maxY) maxY=sugarList[index][natom].coord_orth().y();
                        if(sugarList[index][natom].coord_orth().z() > maxZ) maxZ=sugarList[index][natom].coord_orth().z();
                        if(sugarList[index][natom].coord_orth().x() < minX) minX=sugarList[index][natom].coord_orth().x();
                        if(sugarList[index][natom].coord_orth().y() < minY) minY=sugarList[index][natom].coord_orth().y();
                        if(sugarList[index][natom].coord_orth().z() < minZ) minZ=sugarList[index][natom].coord_orth().z();
                    }

                    x = minX + ((maxX - minX)/2);
                    y = minY + ((maxY - minY)/2);
                    z = minZ + ((maxZ - minZ)/2);


                    // now calculate the correlation between the weighted experimental & calculated maps
                    // maps are scanned only inside a sphere containing the sugar for performance reasons,
                    // although RSCC and <RMS> are restricted to a mask surrounding the model


                    //////// mask calculation //////////

                    clipper::Xmap<double> mask( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

                    clipper::EDcalc_mask<double> masker( ipradius );
                    masker(mask, sugarList[index].atom_list());

                    ////////////////////////////////////

                    clipper::Coord_orth origin(minX-2,minY-2,minZ-2);
                    clipper::Coord_orth destination(maxX+2,maxY+2,maxZ+2);

                    double accum = 0.0;
                    double corr_coeff = 0.0;
                    std::pair<double, double> rscc_and_accum;


                    rscc_and_accum = privateer::cryo_em::calculate_rscc(cryo_em_map, ligandmap, mask, hklinfo, mygrid, origin, destination);
                    
                    corr_coeff = rscc_and_accum.first;
                    accum = rscc_and_accum.second;

                    ligandList[index].second.set_rscc ( corr_coeff );
                    ligandList[index].second.set_accum_score ( accum );
                    ///////////// here we deal with the sugar /////////////

                    std::vector < clipper::MGlycan > list_of_glycans = mgl.get_list_of_glycans();
                    bool found_in_tree = false;

                    for ( int i = 0 ; i < list_of_glycans.size() ; i++ )
                    {
                        std::vector < clipper::MSugar > list_of_sugars = list_of_glycans[i].get_sugars();

                        for ( int j = 0 ; j < list_of_sugars.size() ; j++ )
                        {
                            if ( list_of_sugars[j].id().trim() == ligandList[index].second.id().trim() )
                            {
                                if ( list_of_glycans[i].get_type() == "n-glycan" )
                                {
                                    ligandList[index].second.set_context ( "n-glycan" );
                                }
                                else if ( list_of_glycans[i].get_type() == "c-glycan" )
                                {
                                    ligandList[index].second.set_context ( "c-glycan" );
                                }
                                else if ( list_of_glycans[i].get_type() == "o-glycan" )
                                {
                                    ligandList[index].second.set_context ( "o-glycan" );
                                }
                                else if ( list_of_glycans[i].get_type() == "s-glycan" )
                                {
                                    ligandList[index].second.set_context ( "s-glycan" );
                                }
                                found_in_tree = true;
                                break;
                            }
                        }
                        if ( found_in_tree ) break;
                    }

                    if ( !found_in_tree )
                    {
                        ligandList[index].second.set_context ( "ligand" );
                    }

                    

                    if ( ! ligandList[index].second.ok_with_conformation () )
                        enable_torsions_for.push_back (ligandList[index].second.type().trim());

                    bool occupancy_check = false;
                    std::vector<clipper::MAtom> ringcomponents = ligandList[index].second.ring_members();

                    for ( int i = 0 ; i < ringcomponents.size() ; i++ )
                        if (privateer::util::get_altconformation(ringcomponents[i]) != ' ')
                            occupancy_check = true;

                    ligandList[index].second.set_occupancy_check ( occupancy_check );
                    
                });
                processedMonomers++;

                #if DUMP
                    std::cout << std::endl;
                    DBG << "Processed " << processedMonomers << "/" << ligandList.size() << " monomers..." << std::endl;
                #endif

                if(!clean_output) std::cout << "Processed " << processedMonomers << "/" << ligandList.size() << " monomers..." << std::endl;
            }

            #if DUMP
                std::cout << std::endl;
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
            #endif
            
            while(pool.n_remaining_jobs() > 0)
                pool.sync();
            
            #if DUMP
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
            #endif
            
            privateer::util::print_monosaccharide_summary (batch, showGeom, pos_slash, useMRC, ligandList, output, hklinfo, input_model);
        }
        else
        {
            int processedMonomers = 0;
            for (int index = 0; index < ligandList.size(); index++)
            {
                float x,y,z,maxX,maxY,maxZ,minX,minY,minZ;
                x=y=z=0.0;
                maxX=maxY=maxZ=-999999.0;
                minX=minY=minZ=999999.0;

                for (int natom = 0; natom < sugarList[index].size(); natom++)
                {
                    if(sugarList[index][natom].coord_orth().x() > maxX) maxX=sugarList[index][natom].coord_orth().x(); // calculation of the sugar centre
                    if(sugarList[index][natom].coord_orth().y() > maxY) maxY=sugarList[index][natom].coord_orth().y();
                    if(sugarList[index][natom].coord_orth().z() > maxZ) maxZ=sugarList[index][natom].coord_orth().z();
                    if(sugarList[index][natom].coord_orth().x() < minX) minX=sugarList[index][natom].coord_orth().x();
                    if(sugarList[index][natom].coord_orth().y() < minY) minY=sugarList[index][natom].coord_orth().y();
                    if(sugarList[index][natom].coord_orth().z() < minZ) minZ=sugarList[index][natom].coord_orth().z();
                }

                x = minX + ((maxX - minX)/2);
                y = minY + ((maxY - minY)/2);
                z = minZ + ((maxZ - minZ)/2);


                // now calculate the correlation between the weighted experimental & calculated maps
                // maps are scanned only inside a sphere containing the sugar for performance reasons,
                // although RSCC and <RMS> are restricted to a mask surrounding the model


                //////// mask calculation //////////

                clipper::Xmap<double> mask( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

                clipper::EDcalc_mask<double> masker( ipradius );
                masker(mask, sugarList[index].atom_list());

                ////////////////////////////////////

                clipper::Coord_orth origin(minX-2,minY-2,minZ-2);
                clipper::Coord_orth destination(maxX+2,maxY+2,maxZ+2);

                double accum = 0.0;
                double corr_coeff = 0.0;
                std::pair<double, double> rscc_and_accum;


                rscc_and_accum = privateer::cryo_em::calculate_rscc(cryo_em_map, ligandmap, mask, hklinfo, mygrid, origin, destination);
                
                corr_coeff = rscc_and_accum.first;
                accum = rscc_and_accum.second;

                ligandList[index].second.set_rscc ( corr_coeff );
                ligandList[index].second.set_accum_score ( accum );
                ///////////// here we deal with the sugar /////////////

                std::vector < clipper::MGlycan > list_of_glycans = mgl.get_list_of_glycans();
                bool found_in_tree = false;

                for ( int i = 0 ; i < list_of_glycans.size() ; i++ )
                {
                    std::vector < clipper::MSugar > list_of_sugars = list_of_glycans[i].get_sugars();

                    for ( int j = 0 ; j < list_of_sugars.size() ; j++ )
                    {
                        if ( list_of_sugars[j].id().trim() == ligandList[index].second.id().trim() )
                        {
                            if ( list_of_glycans[i].get_type() == "n-glycan" )
                            {
                                ligandList[index].second.set_context ( "n-glycan" );
                            }
                            else if ( list_of_glycans[i].get_type() == "c-glycan" )
                            {
                                ligandList[index].second.set_context ( "c-glycan" );
                            }
                            else if ( list_of_glycans[i].get_type() == "o-glycan" )
                            {
                                ligandList[index].second.set_context ( "o-glycan" );
                            }
                            else if ( list_of_glycans[i].get_type() == "s-glycan" )
                            {
                                ligandList[index].second.set_context ( "s-glycan" );
                            }
                            found_in_tree = true;
                            break;
                        }
                    }
                    if ( found_in_tree ) break;
                }

                if ( !found_in_tree )
                {
                    ligandList[index].second.set_context ( "ligand" );
                }

                

                if ( ! ligandList[index].second.ok_with_conformation () )
                    enable_torsions_for.push_back (ligandList[index].second.type().trim());

                bool occupancy_check = false;
                std::vector<clipper::MAtom> ringcomponents = ligandList[index].second.ring_members();

                for ( int i = 0 ; i < ringcomponents.size() ; i++ )
                    if (privateer::util::get_altconformation(ringcomponents[i]) != ' ')
                        occupancy_check = true;

                ligandList[index].second.set_occupancy_check ( occupancy_check );

                processedMonomers++;

                #if DUMP
                    std::cout << std::endl;
                    DBG << "Processed " << processedMonomers << "/" << ligandList.size() << " monomers..." << std::endl;
                #endif

                if(!clean_output) std::cout << "Processed " << processedMonomers << "/" << ligandList.size() << " monomers..." << std::endl;
            }
            privateer::util::print_monosaccharide_summary (batch, showGeom, pos_slash, useMRC, ligandList, output, hklinfo, input_model);
        }
    }


    // calculate structure factors
    if ( useMTZ && !useMRC  && !noMaps )
    {
        if (!batch) std::cout << "Done analyzing modelled carbohydrates.\nCalculating structure factors with bulk solvent correction... "; fflush(0);

        clipper::SFcalc_obs_bulk<float> sfcbligands;
        clipper::SFcalc_obs_bulk<float> sfcb;
        clipper::SFcalc_obs_bulk<float> sfcball;

        try
        {   // calculate structure factors with bulk solvent correction
            if(useParallelism)
            {
                pool.push([&sfcbligands, &fc_ligands_bsc, &fobs, &ligandAtoms](int id)
                { 
                    #if DUMP
                        std::cout << std::endl;
                        DBG << "Calculating sfcbligands from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sfcbligands( fc_ligands_bsc, fobs, ligandAtoms );
                });
        
                pool.push([&sfcb, &fc_omit_bsc, &fobs, &mainAtoms](int id)
                { 
                    #if DUMP
                        DBG << "Calculating sfcb from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sfcb( fc_omit_bsc, fobs, mainAtoms );
                });

                pool.push([&sfcball, &fc_all_bsc, &fobs, &allAtoms](int id)
                { 
                    #if DUMP
                        DBG << "Calculating sfcball from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sfcball( fc_all_bsc, fobs, allAtoms );
                });
            }
            else
            {
                sfcbligands( fc_ligands_bsc, fobs, ligandAtoms ); // was fobs_scaled

                sfcb( fc_omit_bsc, fobs, mainAtoms );  // calculation of omit SF with bulk solvent correction

                sfcball( fc_all_bsc, fobs, allAtoms ); // calculation of SF with bulk solvent correction
            }
        }
        catch ( ... )
        {
            if (!batch) std::cout << "\nThe input file has unrecognised atoms. Might cause unexpected results...\n";  // this causes clipper to freak out, so better remove those unknowns
        }

        fc_ligands_bsc[0].set_null();
        fc_omit_bsc[0].set_null();
        fc_all_bsc[0].set_null();

        if (useParallelism)
        {
            #if DUMP
                std::cout << std::endl;
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
            #endif
            
            while(pool.n_remaining_jobs() > 0)
                pool.sync();
            
            #if DUMP
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
            #endif
        }

        if (!batch)
        {
            std::cout << "done." << std::endl << "Computing 2mFo-DFc, mFo-DFc and mFo-omit_DFc maps... ";
            fflush(0);
        }

        clipper::Grid_sampling mygrid( hklinfo.spacegroup(), hklinfo.cell(), hklinfo.resolution() );  // define grid
        clipper::Xmap<float> sigmaa_all_map( hklinfo.spacegroup(), hklinfo.cell(), mygrid );          // define sigmaa best map
        clipper::Xmap<float> sigmaa_dif_map( hklinfo.spacegroup(), hklinfo.cell(), mygrid );          // define sigmaa diff  map
        clipper::Xmap<float> sigmaa_omit_fd( hklinfo.spacegroup(), hklinfo.cell(), mygrid );          // define sigmaa omit diff map
        clipper::Xmap<float> ligandmap( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

        // scale data and flag R-free

        HRI ih;
        clipper::HKL_data<Flag> flag( hklinfo );     // same flag for both calculations, omit absent reflections
        clipper::SFscale_aniso<float> sfscale;


        if(useParallelism)
            {
                pool.push([&sfscale, &fobs_scaled, &fc_all_bsc](int id)
                { 
                    #if DUMP
                        std::cout << std::endl;
                        DBG << "Calculating sfscale from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sfscale( fobs_scaled, fc_all_bsc );
                });

                pool.push([&fobs_scaled, &flag, &ih](int id)
                { 
                    #if DUMP
                        DBG << "Calculating flag[ih].flag() from Thread ID: " << id << '.' << std::endl;
                    #endif

                    for ( ih = flag.first(); !ih.last(); ih.next() ) // we want to use all available reflections
                    {
                        if ( !fobs_scaled[ih].missing() ) flag[ih].flag() = clipper::SFweight_spline<float>::BOTH;
                        else flag[ih].flag() = clipper::SFweight_spline<float>::NONE;
                    }
                });
            }
            else
            {
                sfscale( fobs_scaled, fc_all_bsc );  // anisotropic scaling of Fobs. We scale Fobs to Fcalc instead of scaling our 3 Fcalcs to Fobs

                for ( ih = flag.first(); !ih.last(); ih.next() ) // we want to use all available reflections
                {
                    if ( !fobs_scaled[ih].missing() ) flag[ih].flag() = clipper::SFweight_spline<float>::BOTH;
                    else flag[ih].flag() = clipper::SFweight_spline<float>::NONE;
                }
            }
            
        if (useParallelism)
        {
            #if DUMP
                std::cout << std::endl;
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
            #endif
            
            while(pool.n_remaining_jobs() > 0)
                pool.sync();
            
            #if DUMP
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
            #endif
        }

        double FobsFcalcSum = 0.0;
        double FobsFcalcAllSum = 0.0;
        double FobsSum = 0.0;

        clipper::HKL_data<F_phi> fb_omit( hklinfo ); // new variables for omit sigmaa weighting calculation
        clipper::HKL_data<F_phi> fd_omit( hklinfo );
        clipper::HKL_data<Phi_fom> phiw_omit( hklinfo );
        clipper::HKL_data<F_phi> fb_all( hklinfo ); // variables for all atom sigmaa weighting calculation
        clipper::HKL_data<F_phi> fd_all( hklinfo );
        clipper::HKL_data<Phi_fom> phiw_all( hklinfo );



        if(useParallelism)
        {
            pool.push([&fb_omit, &fd_omit, &phiw_omit, &fobs_scaled, &fc_omit_bsc, &flag, n_refln, n_param](int id)
            { 
                #if DUMP
                    std::cout << std::endl;
                    DBG << "Calculating sfw_omit from Thread ID: " << id << '.' << std::endl;
                #endif
                
                clipper::SFweight_spline<float> sfw_omit (n_refln, n_param );
                sfw_omit( fb_omit, fd_omit, phiw_omit, fobs_scaled, fc_omit_bsc, flag );
            });

            pool.push([&fb_all, &fd_all, &phiw_all, &fobs_scaled, &fc_all_bsc, &flag, n_refln, n_param](int id)
            { 
                #if DUMP
                    DBG << "Calculating sfw_all from Thread ID: " << id << '.' << std::endl;
                #endif
                
                clipper::SFweight_spline<float> sfw_all( n_refln, n_param );
                sfw_all( fb_all,  fd_all,  phiw_all,  fobs_scaled, fc_all_bsc,  flag ); 
            });
        }
        else
        {
            clipper::SFweight_spline<float> sfw_omit (n_refln, n_param );
            sfw_omit( fb_omit, fd_omit, phiw_omit, fobs_scaled, fc_omit_bsc, flag );

            clipper::SFweight_spline<float> sfw_all( n_refln, n_param );
            sfw_all( fb_all,  fd_all,  phiw_all,  fobs_scaled, fc_all_bsc,  flag ); 
        }
        
        // fb:          output best map coefficients
        // fd:          output difference map coefficients
        // phiw:        output phase and fom
        // fobs_scaled: input observed structure factors, previously scaled
        // fc_omit_bsc: input calculated omit data, with bulk solvent correction
        // fc_all_bsc:  input calculated data, with bsc


        if (useParallelism)
        {
            #if DUMP
                std::cout << std::endl;
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
            #endif
            
            while(pool.n_remaining_jobs() > 0)
                pool.sync();
            
            #if DUMP
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
            #endif
        }

        std::vector<double> params( n_param, 2.0 );
        clipper::BasisFn_spline wrk_basis( hklinfo, n_param, 2.0 );

        clipper::TargetFn_scaleF1F2<F_phi,F_sigF> wrk_target_omit( fc_omit_bsc, fobs_scaled ); // was just fobs
        clipper::TargetFn_scaleF1F2<F_phi,F_sigF> wrk_target_all ( fc_all_bsc, fobs_scaled );
        clipper::ResolutionFn wrk_scale_omit( hklinfo, wrk_basis, wrk_target_omit, params );
        clipper::ResolutionFn wrk_scale_all ( hklinfo, wrk_basis, wrk_target_all,  params );

        double Fo, Fc_all, Fc_omit;

        if(useParallelism)
        {
            pool.push([&sigmaa_all_map, &fb_all](int id)
            { 
                #if DUMP
                    std::cout << std::endl;
                    DBG << "Calculating sigmaa_all_map from Thread ID: " << id << '.' << std::endl;
                #endif
                
                sigmaa_all_map.fft_from(fb_all);
            });

            pool.push([&sigmaa_dif_map, &fd_all](int id)
            { 
                #if DUMP
                    DBG << "Calculating sigmaa_dif_map from Thread ID: " << id << '.' << std::endl;
                #endif
                
                sigmaa_dif_map.fft_from(fd_all);
            });     

            pool.push([&sigmaa_omit_fd, &fd_omit](int id)
            { 
                #if DUMP
                    DBG << "Calculating sigmaa_omit_fd from Thread ID: " << id << '.' << std::endl;
                #endif
                
                sigmaa_omit_fd.fft_from(fd_omit);
            });  

            pool.push([&ligandmap, &fc_ligands_bsc](int id)
            { 
                ligandmap.fft_from(fc_ligands_bsc);
                
                #if DUMP
                    DBG << "Calculating ligandmap from Thread ID: " << id << '.' << std::endl;
                #endif
            });

            pool.push([&fobs_scaled, &Fo, &Fc_all, &Fc_omit, &wrk_scale_all, &fc_all_bsc, &wrk_scale_omit, &fc_omit_bsc, &FobsFcalcSum, &FobsFcalcAllSum, &FobsSum](int id)
            { 
                #if DUMP
                    DBG << "Calculating FobsSum, FobsFCalcSum and FobsFcalcAllSum from Thread ID: " << id << '.' << std::endl;
                #endif

                for ( HRI ih = fobs_scaled.first(); !ih.last(); ih.next() )
                {
                    if ( !fobs_scaled[ih].missing() )
                    {
                        Fo = fobs_scaled[ih].f();
                        Fc_all = sqrt ( wrk_scale_all.f(ih) ) * fc_all_bsc[ih].f() ;
                        Fc_omit = sqrt ( wrk_scale_omit.f(ih) ) * fc_omit_bsc[ih].f() ;
                        FobsFcalcSum += fabs( Fo - Fc_omit); // R factor calculation
                        FobsFcalcAllSum += fabs( Fo- Fc_all);
                        FobsSum += Fo;
                    }
                }
            });
        }
        else
        {
                sigmaa_all_map.fft_from( fb_all );  // calculate the maps

                sigmaa_dif_map.fft_from( fd_all );

                sigmaa_omit_fd.fft_from( fd_omit );

                ligandmap.fft_from( fc_ligands_bsc );       // this is the map that will serve as Fc map for the RSCC calculation

                for ( HRI ih = fobs_scaled.first(); !ih.last(); ih.next() )
                {
                    if ( !fobs_scaled[ih].missing() )
                    {
                        Fo = fobs_scaled[ih].f();
                        Fc_all = sqrt ( wrk_scale_all.f(ih) ) * fc_all_bsc[ih].f() ;
                        Fc_omit = sqrt ( wrk_scale_omit.f(ih) ) * fc_omit_bsc[ih].f() ;
                        FobsFcalcSum += fabs( Fo - Fc_omit); // R factor calculation
                        FobsFcalcAllSum += fabs( Fo- Fc_all);
                        FobsSum += Fo;
                    }
                }
        }

        if (useParallelism)
        {
            #if DUMP
                std::cout << std::endl;
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
            #endif
            
            while(pool.n_remaining_jobs() > 0)
                pool.sync();
            
            #if DUMP
                DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
            #endif
        }
    
        if (!batch)
            std::cout << "done." << std::endl;

        if ( output_mtz )
        {
            if (!batch)
            {
                std::cout << "Writing map coefficients to " << output_mapcoeffs_mtz << "... ";
                fflush(0);
            }
            if (useMTZ)
            {
                clipper::CCP4MTZfile mtzout;
                mtzout.open_append(input_reflections_mtz, output_mapcoeffs_mtz );
                mtzout.export_hkl_data( fb_all, "*/*/BEST" );
                mtzout.export_hkl_data( fd_all, "*/*/DIFF" );
                mtzout.export_hkl_data( fd_omit,"*/*/OMIT");
                mtzout.close_append();

                if (!batch)
                    std::cout << "done" << std::endl;
            }
            else
                if (!batch)
                    std::cout << "skipped. You must supply an input MTZ from which columns can be read and transferred to the output MTZ." << std::endl << std::endl;
        }

        if (batch) // create miniMTZ files for ccp4i2
        {
            std::cout << opxtal.crystal_name() << " " << opxtal.project_name() << " " << opdset.dataset_name() << " " << opdset.wavelength() << std::endl;
            clipper::String path = "/" + opxtal.crystal_name() + "/" + opdset.dataset_name() + "/[F,PHI]";

            opmtz_best.open_write ( "FPHIOUT.mtz" );
            opmtz_best.export_hkl_info ( hklinfo );
            opmtz_best.export_crystal ( opxtal, path );
            opmtz_best.export_dataset ( opdset, path );
            opmtz_best.export_hkl_data ( fb_all, path );
            opmtz_best.close_write ();

            opmtz_omit.open_write ( "OMITFPHIOUT.mtz" );
            opmtz_omit.export_hkl_info ( hklinfo );
            opmtz_omit.export_crystal ( opxtal, path );
            opmtz_omit.export_dataset ( opdset, path );
            opmtz_omit.export_hkl_data ( fd_omit, path );
            opmtz_omit.close_write ();
        }

            
        if (!batch)
            printf("\n R-all = %1.3f  R-omit = %1.3f\n", (FobsFcalcAllSum / FobsSum), (FobsFcalcSum / FobsSum));

        if (!batch)
            if (((FobsFcalcAllSum / FobsSum)*10) > hklinfo.resolution().limit() + 0.6)
                std::cout << " Warning: R-work is unusually high. Please ensure that your PDB file contains full B-factors instead of residuals after TLS refinement!" << std::endl;

        float difference = (FobsFcalcSum / FobsSum) - (FobsFcalcAllSum / FobsSum);

        if (( difference > 0.15 ) || (clipper::Util::is_nan((FobsFcalcSum / FobsSum))))
        {
            useSigmaa = true;

            if (!batch)
                std::cout << std::endl << " The studied portions of the model account for a very significant part of the data. Calculating RSCC against a regular 2mFo-DFc map" << std::endl;
        }

        if (!batch)
        {
            std::cout << "\nWriting maps to disk... ";
            fflush(0);
        }

        clipper::CCP4MAPfile sigmaa_all_MapOut;
        clipper::CCP4MAPfile sigmaa_dif_MapOut;
        clipper::CCP4MAPfile sigmaa_omit_fd_MapOut;

        if (allSugars)
            input_ccd_code = "all";

        clipper::Map_stats ms;

        if (useSigmaa)
            ms = clipper::Map_stats(sigmaa_all_map);
        else
            ms = clipper::Map_stats(sigmaa_omit_fd);

        if (!batch)
        {
            if(useParallelism)
            {
                pool.push([&sigmaa_all_MapOut, &sigmaa_all_map](int id)
                { 
                    #if DUMP
                        std::cout << std::endl;
                        DBG << "Writing and outputting sigmaa_all_map to \"sigmaa_best.map\" from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sigmaa_all_MapOut.open_write( "sigmaa_best.map" );      // write maps
                    sigmaa_all_MapOut.export_xmap( sigmaa_all_map );
                    sigmaa_all_MapOut.close_write();
                });

                pool.push([&sigmaa_dif_MapOut, &sigmaa_dif_map](int id)
                { 
                    #if DUMP
                        DBG << "Writing and outputting sigmaa_dif_map to \"sigmaa_diff.map\" from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sigmaa_dif_MapOut.open_write( "sigmaa_diff.map" );
                    sigmaa_dif_MapOut.export_xmap( sigmaa_dif_map );
                    sigmaa_dif_MapOut.close_write();
                });

                pool.push([&sigmaa_omit_fd_MapOut, &sigmaa_omit_fd](int id)
                { 
                    #if DUMP
                        DBG << "Writing and outputting sigmaa_omit_fd to \"sigmaa_omit.map\" from Thread ID: " << id << '.' << std::endl;
                    #endif
                    
                    sigmaa_omit_fd_MapOut.open_write( "sigmaa_omit.map" );
                    sigmaa_omit_fd_MapOut.export_xmap( sigmaa_omit_fd );
                    sigmaa_omit_fd_MapOut.close_write();
                });
            }
            else 
            {
                sigmaa_all_MapOut.open_write( "sigmaa_best.map" );      // write maps
                sigmaa_all_MapOut.export_xmap( sigmaa_all_map );
                sigmaa_all_MapOut.close_write();

                sigmaa_dif_MapOut.open_write( "sigmaa_diff.map" );
                sigmaa_dif_MapOut.export_xmap( sigmaa_dif_map );
                sigmaa_dif_MapOut.close_write();

                sigmaa_omit_fd_MapOut.open_write( "sigmaa_omit.map" );
                sigmaa_omit_fd_MapOut.export_xmap( sigmaa_omit_fd );
                sigmaa_omit_fd_MapOut.close_write();
                
                std::cout << "done." << std::endl;
            }




            
            std::cout << "\n\nDetailed validation data" << std::endl;
            std::cout << "------------------------" << std::endl;
        }
        

        if (!batch)
            printf("\nPDB \t    Sugar   \tRsln\t  Q  \t Phi  \tTheta \tRSCC\t   Detected type   \tCnf\t<mFo>\t<Bfac>\tCtx\t Ok?");
        if (!batch && showGeom)
            printf("\tBond lengths, angles and torsions, reported clockwise with in-ring oxygen as first vertex");
        if (!batch)
            printf("\n----\t------------\t----\t-----\t------\t------\t----\t-------------------\t---\t-----\t------\t---\t-----");
        if (!batch && showGeom)
            printf("\t------------------------------------------------------------------------------------------------------------");
        if (!batch)
            printf("\n");

        // TO DO in medium term: Parallelize, clean up and match this bit to what is currently done with cryo-em maps. Too much code duplication here.
        for (int index = 0; index < ligandList.size(); index++)
        {
            float x,y,z,maxX,maxY,maxZ,minX,minY,minZ;
            x=y=z=0.0;
            maxX=maxY=maxZ=-999999.0;
            minX=minY=minZ=999999.0;

            for (int natom = 0; natom < sugarList[index].size(); natom++)
            {
                if(sugarList[index][natom].coord_orth().x() > maxX) maxX=sugarList[index][natom].coord_orth().x(); // calculation of the sugar centre
                if(sugarList[index][natom].coord_orth().y() > maxY) maxY=sugarList[index][natom].coord_orth().y();
                if(sugarList[index][natom].coord_orth().z() > maxZ) maxZ=sugarList[index][natom].coord_orth().z();
                if(sugarList[index][natom].coord_orth().x() < minX) minX=sugarList[index][natom].coord_orth().x();
                if(sugarList[index][natom].coord_orth().y() < minY) minY=sugarList[index][natom].coord_orth().y();
                if(sugarList[index][natom].coord_orth().z() < minZ) minZ=sugarList[index][natom].coord_orth().z();
            }

            x = minX + ((maxX - minX)/2);
            y = minY + ((maxY - minY)/2);
            z = minZ + ((maxZ - minZ)/2);

            if (batch)
            {
                fprintf(output, "%c%c%c%c\t%s-",input_model[1+pos_slash],input_model[2+pos_slash],input_model[3+pos_slash],input_model[4+pos_slash], ligandList[index].second.type().trim().c_str());
                fprintf(output, "%s-%s   ", ligandList[index].first.c_str(), ligandList[index].second.id().trim().c_str());
            }
            else
            {
                printf("%c%c%c%c\t%s-",input_model[1+pos_slash],input_model[2+pos_slash],input_model[3+pos_slash],input_model[4+pos_slash], ligandList[index].second.type().c_str());
                std::cout << ligandList[index].first << "-" << ligandList[index].second.id().trim() << "  ";
            }

            // now calculate the correlation between the weighted experimental & calculated maps
            // maps are scanned only inside a sphere containing the sugar for performance reasons,
            // although RSCC and <RMS> are restricted to a mask surrounding the model

            double meanDensityExp, meanDensityCalc, num, den1, den2, corr_coeff;
            meanDensityCalc = meanDensityExp = num = den1 = den2 = corr_coeff = 0.0;

            int n_points = 0;

            //////// mask calculation //////////

            clipper::Xmap<float> mask( hklinfo.spacegroup(), hklinfo.cell(), mygrid );

            clipper::EDcalc_mask<float> masker( ipradius );
            masker(mask, sugarList[index].atom_list());

            ////////////////////////////////////

            clipper::Coord_orth origin(minX-2,minY-2,minZ-2);
            clipper::Coord_orth destination(maxX+2,maxY+2,maxZ+2);

            clipper::Xmap_base::Map_reference_coord i0, iu, iv, iw;

            double accum = 0.0;

            // calculation of the mean densities of the calc (ligandmap) and weighted obs (sigmaamap) maps

            std::vector<clipper::Xmap_base::Map_reference_coord> buffer_coord;

            if (useSigmaa)
                i0 = clipper::Xmap_base::Map_reference_coord( sigmaa_all_map, origin.coord_frac(hklinfo.cell()).coord_grid(mygrid) );
            else
                i0 = clipper::Xmap_base::Map_reference_coord( sigmaa_omit_fd, origin.coord_frac(hklinfo.cell()).coord_grid(mygrid) );

            for ( iu = i0; iu.coord().u() <= destination.coord_frac(hklinfo.cell()).coord_grid(mygrid).u(); iu.next_u() )
                for ( iv = iu; iv.coord().v() <= destination.coord_frac(hklinfo.cell()).coord_grid(mygrid).v(); iv.next_v() )
                    for ( iw = iv; iw.coord().w() <= destination.coord_frac(hklinfo.cell()).coord_grid(mygrid).w(); iw.next_w() )
                    {
                        if ( mask[iw] == 1.0)
                        {
                            meanDensityCalc = meanDensityCalc + ligandmap[iw];

                            if (useSigmaa)
                                meanDensityExp = meanDensityExp + sigmaa_all_map[iw];
                            else
                                meanDensityExp = meanDensityExp + sigmaa_omit_fd[iw];

                            n_points++;
                        }
                    }

            accum = meanDensityExp / ms.std_dev();
            accum /= n_points;

            meanDensityCalc = meanDensityCalc / n_points;
            meanDensityExp = meanDensityExp / n_points;

            // calculation of the correlation coefficient between calc (ligandmap) and weighted obs (sigmaamap) maps

            if (useSigmaa)
                i0 = clipper::Xmap_base::Map_reference_coord( sigmaa_all_map, origin.coord_frac(hklinfo.cell()).coord_grid(mygrid) );
            else
                i0 = clipper::Xmap_base::Map_reference_coord( sigmaa_omit_fd, origin.coord_frac(hklinfo.cell()).coord_grid(mygrid) );

            for ( iu = i0; iu.coord().u() <= destination.coord_frac(hklinfo.cell()).coord_grid(mygrid).u(); iu.next_u() )
                for ( iv = iu; iv.coord().v() <= destination.coord_frac(hklinfo.cell()).coord_grid(mygrid).v(); iv.next_v() )
                    for ( iw = iv; iw.coord().w() <= destination.coord_frac(hklinfo.cell()).coord_grid(mygrid).w(); iw.next_w() )
                    {
                        if ( mask[iw] == 1.0)
                        {
                            if (useSigmaa)
                            {
                                num = num + (sigmaa_all_map[iw] - meanDensityExp) * (ligandmap[iw] - meanDensityCalc);
                                den1 = den1 + pow((sigmaa_all_map[iw] - meanDensityExp),2);
                                den2 = den2 + pow((ligandmap[iw] - meanDensityCalc),2);
                            }
                            else
                            {
                                num = num + (sigmaa_omit_fd[iw] - meanDensityExp) * (ligandmap[iw] - meanDensityCalc);
                                den1 = den1 + pow((sigmaa_omit_fd[iw] - meanDensityExp),2);
                                den2 = den2 + pow((ligandmap[iw] - meanDensityCalc),2);
                            }
                        }
                    }

            corr_coeff = num / (sqrt(den1) * sqrt(den2));
            

            ///////////// here we deal with the sugar /////////////

            if (batch)
            {
                std::vector<clipper::ftype> cpParams(10, 0);
                cpParams = ligandList[index].second.cremer_pople_params();
                fprintf(output,"\t%1.2f\t%1.3f\t%3.2f\t",hklinfo.resolution().limit(),cpParams[0],cpParams[1] );    // output cremer-pople parameters
                if ( cpParams[2] == -1 ) fprintf ( output, " --  \t" ); else fprintf ( output, "%3.2f\t", cpParams[2] );
                fprintf(output,"%1.2f\t", corr_coeff);                                              // output RSCC and data resolution
                fprintf(output,"%s\t", ligandList[index].second.type_of_sugar().c_str());           // output the type of sugar, e.g. alpha-D-aldopyranose
                fprintf(output,"%s\t", ligandList[index].second.conformation_name().c_str());       // output a 3 letter code for the conformation
                fprintf(output,"%1.3f \t", accum);
                ligandList[index].second.set_rscc ( corr_coeff );

                float bfac = ligandList[index].second.get_bfactor ();

                fprintf ( output, "%3.2f", bfac ); // output <bfactor>

                std::vector < clipper::MGlycan > list_of_glycans = mgl.get_list_of_glycans();
                bool found_in_tree = false;

                for ( int i = 0 ; i < list_of_glycans.size() ; i++ )
                {
                    std::vector < clipper::MSugar > list_of_sugars = list_of_glycans[i].get_sugars();

                    for ( int j = 0 ; j < list_of_sugars.size() ; j++ )
                    {
                        if ( list_of_sugars[j].id().trim() == ligandList[index].second.id().trim() )
                        {
                            if ( list_of_glycans[i].get_type() == "n-glycan" )
                            {
                                ligandList[index].second.set_context ( "n-glycan" );
                                fprintf ( output, "\t(n) " );
                            }
                            else if ( list_of_glycans[i].get_type() == "c-glycan" )
                            {
                                ligandList[index].second.set_context ( "c-glycan" );
                                fprintf ( output, "\t(c) " );
                            }
                            else if ( list_of_glycans[i].get_type() == "o-glycan" )
                            {
                                ligandList[index].second.set_context ( "o-glycan" );
                                fprintf ( output, "\t(o) " );
                            }
                            else if ( list_of_glycans[i].get_type() == "s-glycan" )
                            {
                                ligandList[index].second.set_context ( "s-glycan" );
                                fprintf ( output, "\t(s) " );
                            }
                            found_in_tree = true;
                            break;
                        }
                    }
                    if ( found_in_tree ) break;
                }

                if ( !found_in_tree )
                {
                    ligandList[index].second.set_context ( "ligand" );
                    fprintf ( output, "\t(l)");
                }

                if (ligandList[index].second.in_database(ligandList[index].second.type().trim()))
                {
                    if ((ligandList[index].second.ring_members().size() == 6 ))
                    {
                        if (ligandList[index].second.is_sane())
                        {
                            if ( ! ligandList[index].second.ok_with_conformation () )
                            {
                                fprintf(output, "\tcheck");
                            }
                            else fprintf(output, "\tyes");
                        }
                        else
                            fprintf (output, "\tno");
                    }
                    else
                        if (ligandList[index].second.is_sane())
                            fprintf(output, "\tyes");
                        else
                        {
                            fprintf(output, "\tno");
                        }
                }
                else
                    fprintf(output, "\tunk");

                if ( ! ligandList[index].second.ok_with_conformation () )
                    enable_torsions_for.push_back (ligandList[index].second.type().trim());

                bool occupancy_check = false;
                std::vector<clipper::MAtom> ringcomponents = ligandList[index].second.ring_members();

                for ( int i = 0 ; i < ringcomponents.size() ; i++ )
                    if (privateer::util::get_altconformation(ringcomponents[i]) != ' ')
                        occupancy_check = true;


                if (showGeom)
                {
                    std::vector<clipper::ftype> rangles = ligandList[index].second.ring_angles();
                    std::vector<clipper::ftype> rbonds  = ligandList[index].second.ring_bonds();
                    std::vector<clipper::ftype> rtorsions = ligandList[index].second.ring_torsions();

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        fprintf(output, "\t%1.2f", rbonds[i]);

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        fprintf(output, "\t%3.1f", rangles[i]);

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        fprintf(output, "\t%3.1f", rtorsions[i]);
                }

                if (occupancy_check)
                    fprintf(output, " (*)");

                fprintf(output, "\n");

            }
            else
            {
                std::vector<clipper::ftype> cpParams(10, 0);
                cpParams = ligandList[index].second.cremer_pople_params();
                printf("\t%1.2f\t%1.3f\t%3.2f\t",hklinfo.resolution().limit(),cpParams[0],cpParams[1]);             // output cremer-pople parameters
                if ( cpParams[2] == -1 ) printf ( " --  \t" ); else printf ( "%3.2f\t", cpParams[2] );
                printf("%1.2f\t", corr_coeff);                                                                                              // output RSCC and data resolution
                printf("%s\t", ligandList[index].second.type_of_sugar().c_str());                   // output the type of sugar, e.g. alpha-D-aldopyranose
                printf("%s\t", ligandList[index].second.conformation_name().c_str());               // output a 3 letter code for the conformation
                printf("%1.3f \t", accum);                                                                                                  // output <mFo>
                ligandList[index].second.set_rscc ( corr_coeff );

                float bfac = 0.0;

                for (int i=0; i < ligandList[index].second.size(); i++)
                    bfac+=ligandList[index].second[i].u_iso();

                bfac /= ligandList[index].second.size();
                bfac  = clipper::Util::u2b(bfac);

                printf ( "%3.2f", bfac );                 // output <Bfactor>

                std::vector < clipper::MGlycan > list_of_glycans = mgl.get_list_of_glycans();
                bool found_in_tree = false;

                for ( int i = 0 ; i < list_of_glycans.size() ; i++ )
                {
                    std::vector < clipper::MSugar > list_of_sugars = list_of_glycans[i].get_sugars();

                    for ( int j = 0 ; j < list_of_sugars.size() ; j++ )
                    {
                        if ( list_of_sugars[j].id().trim() == ligandList[index].second.id().trim() )
                        {
                            if ( list_of_glycans[i].get_type() == "n-glycan" )
                            {
                                ligandList[index].second.set_context ( "n-glycan" );
                                std::cout << "\t(n) ";
                            }
                            else if ( list_of_glycans[i].get_type() == "c-glycan" )
                            {
                                ligandList[index].second.set_context ( "c-glycan" );
                                std::cout << "\t(c) ";
                            }
                            else if ( list_of_glycans[i].get_type() == "o-glycan" )
                            {
                                ligandList[index].second.set_context ( "o-glycan" );
                                std::cout << "\t(o) ";
                            }
                            else if ( list_of_glycans[i].get_type() == "s-glycan" )
                            {
                                ligandList[index].second.set_context ( "s-glycan" );
                                std::cout << "\t(s) ";
                            }
                            found_in_tree = true;
                            break;
                        }
                    }
                    if ( found_in_tree ) break;
                }

                if ( !found_in_tree )
                {
                    ligandList[index].second.set_context ( "ligand" );
                    std::cout << "\t(l) ";
                }

                if (ligandList[index].second.in_database(ligandList[index].second.type().trim()))
                {
                    if ((ligandList[index].second.ring_members().size() == 6 ))
                    {
                        if (ligandList[index].second.is_sane())
                        {
                            if ( ! ligandList[index].second.ok_with_conformation () )
                                printf("\tcheck");
                            else
                                printf("\tyes");
                        }
                        else
                            printf ("\tno");
                    }
                    else
                        if (ligandList[index].second.is_sane())
                            printf("\tyes");
                        else printf("\tno");
                }
                else
                    printf("\tunk");

                if ( ! ligandList[index].second.ok_with_conformation () )
                    enable_torsions_for.push_back (ligandList[index].second.type().trim());

                bool occupancy_check = false;
                std::vector<clipper::MAtom> ringcomponents = ligandList[index].second.ring_members();

                for ( int i = 0 ; i < ringcomponents.size() ; i++ )
                    if (privateer::util::get_altconformation(ringcomponents[i]) != ' ')
                        occupancy_check = true;


                if (showGeom)
                {
                    std::vector<clipper::ftype> rangles = ligandList[index].second.ring_angles();
                    std::vector<clipper::ftype> rbonds  = ligandList[index].second.ring_bonds();
                    std::vector<clipper::ftype> rtorsions = ligandList[index].second.ring_torsions();

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        printf("\t%1.2f", rbonds[i]);

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        printf("\t%3.1f", rangles[i]);

                    for (int i = 0 ; i < ligandList[index].second.ring_members().size(); i++ )
                        printf("\t%3.1f", rtorsions[i]);
                }

                if (occupancy_check)
                    std::cout << " (*)";

                std::cout << std::endl;
            }
        }
    }

    if (useParallelism)
    {
        #if DUMP
            std::cout << std::endl;
            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
        #endif
        
        while(pool.n_remaining_jobs() > 0)
            pool.sync();
        
        #if DUMP
            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
        #endif
    }


    if (!batch)
    {
        printf("\n");
        std::cout << "\nPartially occupied monosaccharides, if any, are marked with an asterisk (*)";
        std::cout << std::endl << std::endl;
    }
    else
        fclose(output);

    std::fstream of_scm; std::fstream of_py;
    of_scm.open("privateer-results.scm", std::fstream::out);
    of_py.open("privateer-results.py", std::fstream::out);

    privateer::coot::insert_coot_prologue_scheme ( of_scm );
    privateer::coot::insert_coot_prologue_python ( of_py );
    if (useMRC && !useMTZ && !noMaps) 
    {
        privateer::coot::insert_coot_files_loadup_scheme (of_scm, input_model, "cryoem_calcmodel.map", "cryoem_diff.map", input_cryoem_map, batch, "input_model_nowater.pdb", check_unmodelled);
        privateer::coot::insert_coot_files_loadup_python (of_py,  input_model, "cryoem_calcmodel.map", "cryoem_diff.map", input_cryoem_map, batch, "input_model_nowater.pdb", check_unmodelled);
    }
    else
    {
        privateer::coot::insert_coot_files_loadup_scheme (of_scm, input_model, "sigmaa_best.map", "sigmaa_diff.map", "sigmaa_omit.map", batch, "input_model_nowater.pdb", check_unmodelled);
        privateer::coot::insert_coot_files_loadup_python (of_py,  input_model, "sigmaa_best.map", "sigmaa_diff.map", "sigmaa_omit.map", batch, "input_model_nowater.pdb", check_unmodelled);
    }
    

    int n_geom = 0, n_anomer = 0, n_config = 0, n_pucker = 0, n_conf = 0;

    int sugar_count = 0;

    for (int k = 0 ; k < ligandList.size() ; k++)
    {
        int n_errors = 0;

        clipper::String diagnostic = ligandList[k].second.type().trim() + "/" + ligandList[k].first + "/" + ligandList[k].second.id().trim();
        clipper::String report = "";
        clipper::String sugarRSCC = clipper::String( ligandList[k].second.get_rscc() );
        sugarRSCC.resize(4);
        diagnostic.append ( ": RSCC=" + sugarRSCC + ", " );

        if (ligandList[k].second.is_supported() )
        {
            if ( ! ligandList[k].second.is_sane() )
            {
                sugar_count++;

                if (!ligandList[k].second.ok_with_anomer())
                {
                    if ( n_errors > 0 )
                    {
                        diagnostic.append(", wrong anomer");
                        report.append(", wrong anomer");
                    }
                    else
                    {
                        diagnostic.append("wrong anomer");
                        report.append("Wrong anomer");
                    }

                    n_errors++;
                    n_anomer++;
                }

                if (!ligandList[k].second.ok_with_chirality())
                {
                    if ( n_errors > 0 )
                    {
                        diagnostic.append(", wrong configuration at " + ligandList[k].second.configurational_carbon().name().trim());
                        report.append(", wrong configuration at " + ligandList[k].second.configurational_carbon().name().trim());
                    }
                    else
                    {
                        diagnostic.append("wrong configuration at " + ligandList[k].second.configurational_carbon().name().trim());
                        report.append("Wrong configuration at " + ligandList[k].second.configurational_carbon().name().trim());
                    }
                    n_errors++;
                    n_config++;
                }

                if ( !ligandList[k].second.ok_with_puckering() )
                {
                    if ( n_errors > 0 )
                    {
                        diagnostic.append(", Q=" + clipper::String(ligandList[k].second.puckering_amplitude()) );
                        report.append(", Q=" + clipper::String(ligandList[k].second.puckering_amplitude()) );
                    }
                    else
                    {
                        diagnostic.append("Q=" + clipper::String( ligandList[k].second.puckering_amplitude() ));
                        report.append("Q=" + clipper::String(ligandList[k].second.puckering_amplitude()) );
                    }

                    n_errors++;
                    n_pucker++;
                }

                if ( !ligandList[k].second.ok_with_conformation() )
                {
                    if ( n_errors > 0 )
                    {
                        diagnostic.append(", conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken") );
                        report.append(", conformation might be mistaken");
                    }
                    else
                    {
                        diagnostic.append("conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken"));
                        report.append("Conformation might be mistaken");
                    }

                    n_errors++;
                    n_conf++;
                }

                privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, ligandList[k].second.ring_centre(), diagnostic );
                privateer::coot::insert_coot_go_to_sugar_python ( of_py, ligandList[k].second.ring_centre(), diagnostic );
            }
            else // sugar is sane, but still need to check higher-energy conformations
            {
                if ( !ligandList[k].second.ok_with_conformation() )
                {
                    if ( n_errors > 0 )
                    {
                        diagnostic.append(", conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken") );
                        report.append(", conformation might be mistaken");
                    }
                    else
                    {
                        diagnostic.append("conformation (" + ligandList[k].second.conformation_name() + clipper::String(") might be mistaken"));
                        report.append("Conformation might be mistaken");
                    }

                    n_errors++;
                    n_conf++;

                    sugar_count++;
                    privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, ligandList[k].second.ring_centre(), diagnostic );
                    privateer::coot::insert_coot_go_to_sugar_python ( of_py, ligandList[k].second.ring_centre(), diagnostic );
                }

            }
        }

        if ( report == "" )
            report = "Ok";

        ligandList[k].second.set_diagnostic ( report );

    }


    if(!blobsProteinBackboneSummaryForCoot.empty() && check_unmodelled)
    {
        for(int type = 0; type < blobsProteinBackboneSummaryForCoot.size(); type++)
            {
                if(type == 0 && !blobsProteinBackboneSummaryForCoot[type].empty())
                {
                    for(int i = 0; i < blobsProteinBackboneSummaryForCoot[type].size(); i++)
                    {
                            // Printing vector tuples 
                        clipper::String diagnostic = "N-linked blob: " + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).id() + "-" + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).type() + " of Chain " + std::get<0>(blobsProteinBackboneSummaryForCoot[type][i]) + " = " + clipper::String(std::get<2>(blobsProteinBackboneSummaryForCoot[type][i])) + " - might be in proximity of unmodelled glycosylation site";
                        
                        clipper::MAtom DUMAtom;
                        DUMAtom = std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).find(" DUM", clipper::MM::ANY);

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, DUMAtom.coord_orth(), diagnostic);
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, DUMAtom.coord_orth(), diagnostic);
                    }
                }

                if(type == 1 && !blobsProteinBackboneSummaryForCoot[type].empty())
                {
                    for(int i = 0; i < blobsProteinBackboneSummaryForCoot[type].size(); i++)
                    {
                        clipper::String diagnostic = "C-linked blob: " + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).id() + "-" + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).type() + " of Chain " + std::get<0>(blobsProteinBackboneSummaryForCoot[type][i]) + " = " + clipper::String(std::get<2>(blobsProteinBackboneSummaryForCoot[type][i])) + " - might be in proximity of unmodelled glycosylation site";
                        
                        clipper::MAtom DUMAtom;
                        DUMAtom = std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).find(" DUM", clipper::MM::ANY);

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, DUMAtom.coord_orth(), diagnostic);
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, DUMAtom.coord_orth(), diagnostic);                       
                    }
                }

                if(type == 2 && !blobsProteinBackboneSummaryForCoot[type].empty())
                {
                    for(int i = 0; i < blobsProteinBackboneSummaryForCoot[type].size(); i++)
                    {
                        clipper::String diagnostic = "O-linked blob: " + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).id() + "-" + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).type() + " of Chain " + std::get<0>(blobsProteinBackboneSummaryForCoot[type][i]) + " = " + clipper::String(std::get<2>(blobsProteinBackboneSummaryForCoot[type][i])) + " - might be in proximity of unmodelled glycosylation site";
                        
                        clipper::MAtom DUMAtom;
                        DUMAtom = std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).find(" DUM", clipper::MM::ANY);

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, DUMAtom.coord_orth(), diagnostic);
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, DUMAtom.coord_orth(), diagnostic);                        
                    }
                }

                if(type == 3 && !blobsProteinBackboneSummaryForCoot[type].empty())
                {
                    for(int i = 0; i < blobsProteinBackboneSummaryForCoot[type].size(); i++)
                    {
                        clipper::String diagnostic = "S-linked blob: " + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).id() + "-" + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).type() + " of Chain " + std::get<0>(blobsProteinBackboneSummaryForCoot[type][i]) + " = " + clipper::String(std::get<2>(blobsProteinBackboneSummaryForCoot[type][i])) + " - might be in proximity of unmodelled glycosylation site";
                        
                        clipper::MAtom DUMAtom;
                        DUMAtom = std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).find(" DUM", clipper::MM::ANY);

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, DUMAtom.coord_orth(), diagnostic);
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, DUMAtom.coord_orth(), diagnostic);                        
                    }
                }   

                if(type == 4 && !blobsProteinBackboneSummaryForCoot[type].empty())
                {
                    for(int i = 0; i < blobsProteinBackboneSummaryForCoot[type].size(); i++)
                    {
                        clipper::String diagnostic = "PNGase blob: " + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).id() + "-" + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).type() + " of Chain " + std::get<0>(blobsProteinBackboneSummaryForCoot[type][i]) + " = " + clipper::String(std::get<2>(blobsProteinBackboneSummaryForCoot[type][i])) + " - might be in proximity of unmodelled glycosylation site";
                        
                        clipper::MAtom DUMAtom;
                        DUMAtom = std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).find(" DUM", clipper::MM::ANY);

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, DUMAtom.coord_orth(), diagnostic);
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, DUMAtom.coord_orth(), diagnostic);                        
                    }
                }   
                if(type == 5 && !blobsProteinBackboneSummaryForCoot[type].empty())
                {
                    for(int i = 0; i < blobsProteinBackboneSummaryForCoot[type].size(); i++)
                    {
                        clipper::String diagnostic = "Glycan chain blob: " + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).id() + "-" + std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).type() + " of Chain " + std::get<0>(blobsProteinBackboneSummaryForCoot[type][i]) + " = " + clipper::String(std::get<2>(blobsProteinBackboneSummaryForCoot[type][i])) + " - might be in proximity of unmodelled glycosylation site";
                        
                        clipper::MAtom DUMAtom;
                        DUMAtom = std::get<1>(blobsProteinBackboneSummaryForCoot[type][i]).find(" DUM", clipper::MM::ANY);

                        privateer::coot::insert_coot_go_to_sugar_scheme ( of_scm, DUMAtom.coord_orth(), diagnostic);
                        privateer::coot::insert_coot_go_to_sugar_python ( of_py, DUMAtom.coord_orth(), diagnostic);                        
                    }
                }                                          

            }
    }

    
    privateer::coot::insert_coot_epilogue_scheme ( of_scm );
    privateer::coot::insert_coot_epilogue_python ( of_py );
    of_scm.close();
    of_py.close();

    std::cout << "SUMMARY: " << std::endl << std::endl ;
    std::cout << "   Wrong anomer: " << n_anomer << std::endl;
    std::cout << "   Wrong configuration: " << n_config << std::endl;
    std::cout << "   Unphysical puckering amplitude: " << n_pucker << std::endl;
    std::cout << "   In higher-energy conformations: " << n_conf << std::endl;
    std::cout << std::endl;
    std::cout << "   Privateer has identified " << n_anomer + n_config + n_pucker + n_conf;
    std::cout << " issues, with " << sugar_count << " of " << ligandList.size() << " sugars affected." << std::endl;

    privateer::util::print_XML(ligandList, list_of_glycans, list_of_glycans_associated_to_permutations, input_model, jsonObject);

    

    if ( enable_torsions_for.size() > 0 )
    {
        privateer::util::write_refmac_keywords ( enable_torsions_for );
        privateer::util::write_libraries( enable_torsions_for );
    }


    if (useParallelism)
    {
        
        #if DUMP
            std::cout << std::endl;
            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << std::endl;
        #endif
        
        while(pool.n_remaining_jobs() > 0)
            pool.sync();
        
        #if DUMP
            DBG << "Number of jobs in the queue: " << pool.n_remaining_jobs() << " after sync operation!" << std::endl;
        #endif
    }


    prog.set_termination_message( "Normal termination" );
    system("touch scored");
    return 0;
}
