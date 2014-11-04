/*! \file clipper-glyco_data.cpp
Implementation file for the sugar database */

//C (C) 2014 Jon Agirre & Kevin Cowtan, The University of York
//L
//L  This library is free software and is distributed under the terms
//L  and conditions of version 2.1 of the GNU Lesser General Public
//L  Licence (LGPL) with the following additional clause:
//L
//L     `You may also combine or link a "work that uses the Library" to
//L     produce a work containing portions of the Library, and distribute
//L     that work under terms of your choice, provided that you give
//L     prominent notice with each copy of the work that the specified
//L     version of the Library is used in it, and that you include or
//L     provide public access to the complete corresponding
//L     machine-readable source code for the Library including whatever
//L     changes were used in the work. (i.e. If you make changes to the
//L     Library you must distribute those, but you do not need to
//L     distribute source or object code to those portions of the work
//L     not covered by this licence.)'
//L
//L  Note that this clause grants an additional right and does not impose
//L  any additional restriction, and so does not affect compatibility
//L  with the GNU General Public Licence (GPL). If you wish to negotiate
//L  other terms, please contact the maintainer.
//L
//L  You can redistribute it and/or modify the library under the terms of
//L  the GNU Lesser General Public License as published by the Free Software
//L  Foundation; either version 2.1 of the License, or (at your option) any
//L  later version.
//L
//L  This library is distributed in the hope that it will be useful, but
//L  WITHOUT ANY WARRANTY; without even the implied warranty of
//L  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//L  Lesser General Public License for more details.
//L
//L  You should have received a copy of the CCP4 licence and/or GNU
//L  Lesser General Public License along with this library; if not, write
//L  to the CCP4 Secretary, Daresbury Laboratory, Warrington WA4 4AD, UK.
//L  The GNU Lesser General Public can also be obtained by writing to the
//L  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//L  MA 02111-1307 USA


#include "clipper-glyco_data.h"

namespace clipper
{

namespace data
{

const clipper::String conformational_landscape[] =
{
    "Pln", "4c1", "1c4",
    "3Ob", "b25", "14b", "b3O", "25b", "b14",
    "Oev", "ev5", "4ev", "ev3", "2ev", "ev1", "3ev", "ev2", "1ev", "evO", "5ev", "ev4",
    "Oh5", "4h5", "4h3", "2h3", "2h1", "Oh1", "3h2", "1h2", "1hO", "5hO", "5h4", "3h4",
    "Os2", "1s5", "1s3", "2sO", "5s1", "3s1",
    "3t2", "3ev", "3t4", "ev4", "Ot4", "Oev", "Ot1", "ev1", "2t1", "2ev", "2t3", "ev3", "4t3", "4ev", "4tO", "evO", "1tO", "1ev", "1t2", "ev2"
};

const clipper::String iupac_conformational_landscape[] =
{
    "Planar ring", "&lt;sup&gt;4&lt;/sup&gt;C&lt;sub&gt;1&lt;/sub&gt;", "<sup>1</sup>C<sub>4",
    "<sup>3,O</sup>B", "B<sub>2,5</sub>", "<sup>1,4</sup>B", "B<sub>3,O</sub>", "<sup>2,5</sup>B", "B<sub>1,4</sub>",
    "<sup>O</sup>E", "E<sub>5</sub>", "<sup>4</sup>E", "E<sub>3</sub>", "<sup>2</sup>E", "E<sub>1</sub>", "<sup>3</sup>E", 
    "E<sub>2</sub>", "<sup>1</sup>E", "E<sub>O</sub>", "<sup>5</sup>E", "E<sub>4</sub>",
    "<sup>O</sup>H<sub>5</sub>", "<sup>4</sup>H<sub>5</sub>", "<sup>4</sup>H<sub>3</sub>", "<sup>2</sup>H<sub>3</sub>", 
    "<sup>2</sup>H<sub>1</sub>",
    "<sup>O</sup>H<sub>1</sub>", "<sup>3</sup>H<sub>2</sub>", "<sup>1</sup>H<sub>2</sub>", "<sup>1</sup>H<sub>O</sub>", 
    "<sup>5</sup>H<sub>O</sub>", 
    "<sup>5</sup>H<sub>4</sub>", "<sup>3</sup>H<sub>4</sub>",
    "<sup>O</sup>S<sub>2</sub>", "<sup>1</sup>S<sub>5</sub>", "<sup>1</sup>S<sub>3</sub>", "<sup>2</sup>S<sub>O</sub>", 
    "<sup>5</sup>S<sub>1</sub>", "<sup>3</sup>S<sub>1</sub>",
    "<sup>3</sup>T<sub>2</sub>", "<sup>3</sup>E","<sup>3</sup>T<sub>4</sub>", "E<sub>4</sub>","<sup>O</sup>T<sub>4</sub>", 
    "<sup>O</sup>E","<sup>O</sup>T<sub>1</sub>", "E<sub>1</sub>","<sup>2</sup>T<sub>1</sub>", "<sup>2</sup>E",
    "<sup>2</sup>T<sub>3</sub>", "E<sub>3</sub>","<sup>4</sup>T<sub>3</sub>", "<sup>4</sup>E","<sup>4</sup>T<sub>O</sub>", 
    "E<sub>O</sub>","<sup>1</sup>T<sub>O</sub>", "<sup>1</sup>E","<sup>1</sup>T<sub>2</sub>", "E<sub>2</sub>"
};

const sugar_database_entry sugar_database[] =
{
    { "13A" ,    "B",    "L",    "7-(3,4-DIHYDROXY-5R-HYDROXYMETHYLTETRAHYDROFU" ,  "O1 C2 C3 C4 C5"    },
    { "147" ,	 "B", 	 "D", 	 "1-O-[P-NITROPHENYL]-BETA-D-GALACTOSE" ,           "O5 C1 C2 C3 C4 C5" },
    { "149" ,	 "N", 	 "D", 	 "D-GALACTONOLACTONE" ,				    "O5 C1 C2 C3 C4 C5" },
    { "15L" ,	 "N", 	 "D", 	 "GALACTARO-1,5-LACTONE" , 			    "O5 C1 C2 C3 C4 C5" },
    { "16G" ,	 "A", 	 "D", 	 "N-ACETYL-D-GLUCOSAMINE-6-PHOSPHATE" , 	    "O5 C1 C2 C3 C4 C5" },
    { "18D" ,	 "A", 	 "D", 	 "3,5-DIDEOXY-5-(PROPANOYLAMINO)-D-GLYCERO-ALPH" ,  "O6 C2 C3 C4 C5 C6" },
    { "1GL" ,	 "A", 	 "D", 	 "4-O-METHYL-2,6-DIDEOXY-ALPHA-D-GALACTO-HEXOPY" ,  "O5 C1 C2 C3 C4 C5" },
    { "1GN" ,	 "B", 	 "D", 	 "2-DEOXY-2-AMINOGALACTOSE" , 			    "O5 C1 C2 C3 C4 C5" },
    { "289" ,	 "A", 	 "D", 	 "D-GLYCERO-ALPHA-D-MANNO-HEPTOPYRANOSE" , 	    "O5 C1 C2 C3 C4 C5" },
    { "291" ,	 "A", 	 "D", 	 "PROP-2-EN-1-YL7-O-CARBAMOYL-L-GLYCERO-ALPHA-" ,   "O5 C1 C2 C3 C4 C5" },
    { "293" ,	 "B", 	 "L", 	 "(2S,4R,5S,6R)-6-((S)-1,2-DIHYDROXYETHYL)TETRAH" , "O5 C1 C2 C3 C4 C5" },
    { "2DG" ,	 "B", 	 "D", 	 "2-DEOXY-BETA-D-GALACTOSE" , 	 		    "O5 C1 C2 C3 C4 C5" },
    { "2DR" ,	 "B", 	 "D", 	 "2-DEOXY-BETA-D-ERYTHRO-PENTOFURANOSE" , 	    "O4 C1 C2 C3 C4" 	},
    { "2FG" ,	 "B", 	 "D", 	 "2-FLUORO-2-DEOXY-BETA-D-GALACTOPYRANOSE" , 	    "O5 C1 C2 C3 C4 C5" },
    { "2GL" ,	 "A", 	 "D", 	 "4-O-ACETYL-2,6-DIDEOXY-ALPHA-D-GALACTO-HEXOPY" ,  "O5 C1 C2 C3 C4 C5" },
    { "2GS" ,	 "A", 	 "D", 	 "2-O-METHYL-ALPHA-D-GALACTOPYRANOSE" , 	    "O5 C1 C2 C3 C4 C5" },
    { "2M5" ,	 "A", 	 "D", 	 "METHYL7-DEOXY-L-GLYCERO-ALPHA-D-MANNO-HEPTOP" ,   "O5 C1 C2 C3 C4 C5" },
    { "3FM" ,	 "A", 	 "D", 	 "3-O-FORMAMIDO-ALPHA-D-MANNOPYRANOSIDE" ,          "O5 C1 C2 C3 C4 C5" },
    { "3HD" ,	 "A", 	 "D", 	 "3-O-METHYL-O-ALPHA-D-MANNOPYRANOSYL" ,            "O5 C1 C2 C3 C4 C5" },
    { "3MG" ,	 "B", 	 "D", 	 "3-O-METHYL-BETA-D-GLUCOPYRANOSE" ,                "O5 C1 C2 C3 C4 C5" },
    { "42D" ,	 "A", 	 "D", 	 "3,5-DIDEOXY-5-[(METHOXYCARBONYL)AMINO]-D-GLYC" ,  "O6 C2 C3 C4 C5 C6" },
    { "445" ,	 "B", 	 "D", 	 "N-[OXO(PHENYLAMINO)ACETYL]-BETA-D-GLUCOPYRANO" ,  "O5 C1 C2 C3 C4 C5" },
    { "46M" ,	 "A", 	 "D", 	 "(4AR,6R,7S,8R,8AS)-HEXAHYDRO-6,7,8-TRIHYDROXY" ,  "O5 C1 C2 C3 C4 C5" },
    { "475" ,	 "B", 	 "D", 	 "N-[OXO(PYRIDIN-2-YLAMINO)ACETYL]-BETA-D-GLUCO" ,  "O5 C1 C2 C3 C4 C5" },
    { "49A" ,	 "N", 	 "D", 	 "4,9-AMINO-2,4-DEOXY-2,3-DEHYDRO-N-ACETYL-NEUR" ,  "O6 C2 C3 C4 C5 C6" },
    { "4AM" ,	 "A", 	 "D", 	 "N4-AMINO-2-DEOXY-2,3-DEHYDRO-N-NEURAMINICACID" ,  "O6 C2 C3 C4 C5 C6" },
    { "4GP" ,	 "B", 	 "D", 	 "N-(BETA-D-GLUCOPYRANOSYL)OXAMICACID" ,            "O5 C1 C2 C3 C4 C5" },
    { "6GP" ,	 "B", 	 "D", 	 "METHYL-N-(BETA-D-GLUCOPYRANOSYL)OXAMATE" ,        "O5 C1 C2 C3 C4 C5" },
    { "6MN" ,	 "A", 	 "D", 	 "2-AMINO-2-DEOXY-6-O-PHOSPHONO-ALPHA-D-MANNOPY" ,  "O5 C1 C2 C3 C4 C5" },
    { "7GP" ,	 "B", 	 "D", 	 "ETHYL-N-(BETA-D-GLUCOPYRANOSYL)OXAMATE" ,         "O5 C1 C2 C3 C4 C5" },
    { "7JZ" ,	 "B", 	 "D", 	 "2-DEOXY-2,2-DIFLUORO-BETA-D-LYXO-HEXOPYRANOSE" ,  "O5 C1 C2 C3 C4 C5" },
    { "8GP" ,	 "B", 	 "D", 	 "N-(BETA-D-GLUCOPYRANOSYL)-N'-CYCLOPROPYLOXAL" ,   "O5 C1 C2 C3 C4 C5" },
    { "9AM" ,	 "A", 	 "D", 	 "9-AMINO-2-DEOXY-2,3-DEHYDRO-N-ACETYL-NEURAMIN" ,  "O6 C2 C3 C4 C5 C6" },
    { "A6P" ,	 "A", 	 "D", 	 "6-O-PHOSPHONO-ALPHA-D-ALLOPYRANOSE" ,             "O5 C1 C2 C3 C4 C5" },
    { "AAL" ,	 "A", 	 "L", 	 "3,6-ANHYDRO-L-GALACTOSE" ,                        "O5 C1 C2 C3 C4 C5" },
    { "ABE" ,	 "A", 	 "D", 	 "ABEQUOSE" ,                                       "O5 C1 C2 C3 C4 C5" },
    { "ADA" ,	 "A", 	 "D", 	 "ALPHA-D-GALACTOPYRANURONICACID" ,                 "O5 C1 C2 C3 C4 C5" },
    { "AFP" ,	 "A", 	 "D", 	 "ALPHAFRUCTOSE1,6-DIPHOSPHATE" ,                   "O5 C2 C3 C4 C5"    },
    { "AGL" ,	 "A", 	 "D", 	 "4,6-DIDEOXY-4-AMINO-ALPHA-D-GLUCOSE" ,            "O5 C1 C2 C3 C4 C5" },
    { "AH8" ,	 "A", 	 "L", 	 "(2R,3R,4R,5S)-2-AZIDO-5-(HYDROXYMETHYL)TETRAH" ,  "O4 C1 C2 C3 C4"    },
    { "AHG" ,	 "B", 	 "D", 	 "2,5-ANHYDROGLUCITOL-1,6-BIPHOSPHATE" ,            "O5 C2 C3 C4 C5"    },
    { "AMG" ,	 "A", 	 "D", 	 "ALPHA-METHYL-D-GALACTOSIDE" ,                     "O5 C1 C2 C3 C4 C5" },
    { "AMN" ,	 "A", 	 "D", 	 "9-DEOXY-9-AMINO-2-O-METHYL-5-N-ACETYL-ALPHA-D" ,  "O6 C2 C3 C4 C5 C6" },
    { "AMU" ,	 "B", 	 "N", 	 "BETA-N-ACETYLMURAMICACID" ,                       "O5 C1 C2 C3 C4 C5" },
    { "AMV" ,	 "B", 	 "D", 	 "METHYL2-(ACETYLAMINO)-3-O-[(1R)-1-CARBOXYETH" ,   "O5 C1 C2 C3 C4 C5" },
    { "ANA" ,	 "A", 	 "D", 	 "4-O-ACETYL-ALPHA-2-OMETHYL-5-N-ACETYL-ALPHA-D" ,  "O6 C2 C3 C4 C5 C6" },
    { "ARA" ,	 "A", 	 "L", 	 "ALPHA-L-ARABINOSE" ,                              "O5 C1 C2 C3 C4 C5" },
    { "ARB" ,	 "B", 	 "L", 	 "BETA-L-ARABINOSE" ,                               "O5 C1 C2 C3 C4 C5" },
    { "ARW" ,	 "B", 	 "D", 	 "METHYLBETA-D-ARABINOPYRANOSIDE" ,                 "O5 C1 C2 C3 C4 C5" },
    { "ASO" ,	 "N", 	 "D", 	 "1,5-ANHYDROSORBITOL" ,                            "O5 C1 C2 C3 C4 C5" },
    { "AXP" ,	 "B", 	 "D", 	 "4-ACETAMIDO-2,4-DIDEXOY-D-GLYCERO-BETA-D-GALA" ,  "O6 C2 C3 C4 C5 C6" },
    { "AXR" ,	 "A", 	 "D", 	 "METHYLALPHA-D-ARABINOFURANOSIDE" ,                "O4 C1 C2 C3 C4"    },
    { "B16" ,	 "B", 	 "D", 	 "1,6-DI-O-PHOSPHONO-BETA-D-GLUCOPYRANOSE" ,        "O5 C1 C2 C3 C4 C5" },
    { "BDF" ,	 "B", 	 "D", 	 "BETA-D-FRUCTOPYRANOSE" ,                          "O6 C2 C3 C4 C5 C6" },
    { "BDG" ,	 "A", 	 "D", 	 "O-2,6-DIAMINO-2,6-DIDEOXY-ALPHA-D-GLUCOPYRANOSE" ,"O5 C1 C2 C3 C4 C5" },
    { "BDP" ,	 "B", 	 "D", 	 "BETA-D-GLUCOPYRANURONICACID" ,                    "O5 C1 C2 C3 C4 C5" },
    { "BEM" ,	 "B", 	 "D", 	 "BETA-D-MANNURONICACID" ,                          "O5 C1 C2 C3 C4 C5" },
    { "BG6" ,	 "B", 	 "D", 	 "BETA-D-GLUCOSE-6-PHOSPHATE" ,                     "O5 C1 C2 C3 C4 C5" },
    { "BGC" ,	 "B", 	 "D", 	 "BETA-D-GLUCOSE" ,                                 "O5 C1 C2 C3 C4 C5" },
    { "BGN" ,	 "B", 	 "D", 	 "N-BUTANOYL-2-AMINO-2-DEOXY-GLUCOPYRANOSIDE" ,     "O5 C1 C2 C3 C4 C5" },
    { "BGP" ,	 "B", 	 "D", 	 "BETA-GALACTOSE-6-PHOSPHATE" ,                     "O5 C1 C2 C3 C4 C5" },
    { "BGS" ,	 "B", 	 "D", 	 "BETA-D-GLUCOPYRANOSYLSULFONYLETHANE" ,            "O5 C1 C2 C3 C4 C5" },
    { "BM3" ,	 "A", 	 "D", 	 "2-(ACETYLAMINO)-2-DEOXY-ALPHA-D-MANNOPYRANOSE" ,  "O5 C1 C2 C3 C4 C5" },
    { "BMA" ,	 "B", 	 "D", 	 "BETA-D-MANNOSE" ,                                 "O5 C1 C2 C3 C4 C5" },
    { "BMX" ,	 "A", 	 "D", 	 "2-(ACETYLAMINO)-2-DEOXY-6-O-PHOSPHONO-ALPHA-D" ,  "O5 C1 C2 C3 C4 C5" },
    { "BXX" ,	 "B", 	 "D", 	 "BETA-D-ARABINOFURANOSE" ,                         "O4 C1 C2 C3 C4"    },
    { "BXY" ,	 "A", 	 "D", 	 "ALPHA-D-ARABINOFURANOSE" ,                        "O4 C1 C2 C3 C4"    },
    { "C3X" ,	 "B", 	 "D", 	 "2,3-EPOXYPROPYL-BETA-D-XYLOSIDE" ,                "O5 C1 C2 C3 C4 C5" },
    { "C4X" ,	 "B", 	 "D", 	 "3,4-EPOXYBUTYL-BETA-D-XYLOSIDE" ,                 "O5 C1 C2 C3 C4 C5" },
    { "C5X" ,	 "B", 	 "D", 	 "4,5-EPOXYPENTYL-BETA-D-XYLOSIDE" ,                "O5 C1 C2 C3 C4 C5" },
    { "CBF" ,	 "B", 	 "D", 	 "C-(1-HYDROGYL-BETA-D-GLUCOPYRANOSYL)FORMAMID" ,   "O5 C1 C2 C3 C4 C5" },
    { "CDG" ,	 "B", 	 "D", 	 "METHYL4,6-O-[(1R)-1-CARBOXYETHYLIDENE]-BETA-" ,   "O5 C1 C2 C3 C4 C5" },
    { "CEG" ,	 "B", 	 "D", 	 "4,6-O-(1-CARBOXYETHYLIDENE)-BETA-D-GLUCOSE" ,     "O5 C1 C2 C3 C4 C5" },
    { "CGF" ,	 "A", 	 "D", 	 "C-(1-AZIDO-ALPHA-D-GLUCOPYRANOSYL)FORMAMIDE" ,    "O5 C1 C2 C3 C4 C5" },
    { "CNP" ,	 "N", 	 "N", 	 "2-PROPENYL-N-ACETYL-NEURAMICACID" ,               "O6 C2 C3 C4 C5 C6" },
    { "CR1" ,	 "B", 	 "D", 	 "1-DEOXY-1-METHOXYCARBAMIDO-BETA-D-GLUCOPYRANO" ,  "O5 C1 C2 C3 C4 C5" },
    { "CR6" ,	 "B", 	 "D", 	 "1-DEOXY-1-ACETYLAMINO-BETA-D-GLUCO-2-HEPTULOP" ,  "O5 C1 C2 C3 C4 C5" },
    { "CRA" ,	 "B", 	 "D", 	 "1-DEOXY-1-METHOXYCARBAMIDO-BETA-D-GLUCO-2-HEP" ,  "O5 C1 C2 C3 C4 C5" },
    { "D6G" ,	 "A", 	 "D", 	 "2-DEOXY-6-O-PHOSPHONO-ALPHA-D-ARABINO-HEXOPYR" ,  "O5 C1 C2 C3 C4 C5" },
    { "DAG" ,	 "B", 	 "D", 	 "4,6-DIDEOXY-4-AMINO-BETA-D-GLUCOPYRANOSIDE" ,     "O5 C1 C2 C3 C4 C5" },
    { "DAN" ,	 "N", 	 "D", 	 "2-DEOXY-2,3-DEHYDRO-N-ACETYL-NEURAMINICACID" ,    "O6 C2 C3 C4 C5 C6" },
    { "DDA" ,	 "B", 	 "D", 	 "2,6-DIDEOXY-BETA-D-GLUCOSE" ,                     "O5 C1 C2 C3 C4 C5" },
    { "DDL" ,	 "B", 	 "D", 	 "2,6-DIDEOXY-BETA-D-GALACTOSE" ,                   "O5 C1 C2 C3 C4 C5" },
    { "DEG" ,	 "A", 	 "D", 	 "BUTYLALPHA-D-MANNOPYRANOSIDE" ,                   "O5 C1 C2 C3 C4 C5" },
    { "DGS" ,	 "A", 	 "D", 	 "3,6-ANHYDRO-D-GALACTOSE-2-SULFATE" ,              "O5 C1 C2 C3 C4 C5" },
    { "DK4" ,	 "B", 	 "D", 	 "1-(3-DEOXY-3-FLUORO-BETA-D-GLUCOPYRANOSYL)-5-" ,  "O5 C1 C2 C3 C4 C5" },
    { "DK5" ,	 "B", 	 "D", 	 "1-(2,3-DIDEOXY-3-FLUORO-BETA-D-ARABINO-HEXOPY" ,  "O5 C1 C2 C3 C4 C5" },
    { "DKX" ,	 "B", 	 "D", 	 "1-(3-DEOXY-3-FLUORO-BETA-D-GLUCOPYRANOSYL)PYR" ,  "O5 C1 C2 C3 C4 C5" },
    { "DKY" ,	 "B", 	 "D", 	 "1-(3-DEOXY-3-FLUORO-BETA-D-GLUCOPYRANOSYL)-4-" ,  "O5 C1 C2 C3 C4 C5" },
    { "DKZ" ,	 "B", 	 "D", 	 "4-AMINO-1-(3-DEOXY-3-FLUORO-BETA-D-GLUCOPYRAN" ,  "O5 C1 C2 C3 C4 C5" },
    { "DL6" ,	 "B", 	 "D", 	 "2-AZIDO-N-((2R,3R,4S,5S,6R)-3,4,5-TRIHYDROXY-" ,  "O5 C1 C2 C3 C4 C5" },
    { "DLF" ,	 "A", 	 "L", 	 "2-DEOXY-ALPHA-L-FUCOPYRANOSIDE" ,                 "O5 C1 C2 C3 C4 C5" },
    { "DLG" ,	 "B", 	 "D", 	 "HEXYL3-DEOXY-BETA-D-GALACTOPYRANOSE" ,            "O5 C1 C2 C3 C4 C5" },
    { "DO8" ,	 "A", 	 "D", 	 "3-DEOXY-D-MANNO-2-OCTULOSONATE-8-PHOSPHATE" ,     "O6 C2 C3 C4 C5 C6" },
    { "DRI" ,	 "B", 	 "D", 	 "4-O-METHYL-2,6-DIDEOXY-BETA-D-GLUCOSE" ,          "O5 C1 C2 C3 C4 C5" },
    { "DSR" ,	 "B", 	 "D", 	 "2,6-DIDEOXY-4-THIO-BETA-D-ALLOSEPYRANOSIDE" ,     "O5 C1 C2 C3 C4 C5" },
    { "DVC" ,	 "N", 	 "N", 	 "(2R,4S,6S)-4-AZANYL-4,6-DIMETHYL-OXANE-2,5,5-" ,  "O5 C1 C2 C3 C4 C5" },
    { "E5G" ,	 "A", 	 "D", 	 "5-HYDROXYPENTYLALPHA-D-GLUCOPYRANOSIDE" ,         "O5 C1 C2 C3 C4 C5" },
    { "EAG" ,	 "B", 	 "D", 	 "2-AMINOETHYL2-(ACETYLAMINO)-2-DEOXY-BETA-D-G" ,   "O5 C1 C2 C3 C4 C5" },
    { "EBG" ,	 "A", 	 "D", 	 "2-HYDROXYMETHYL-6-(2-OXIRANYL-ETHOXY)-TETRAHY" ,  "O5 C1 C2 C3 C4 C5" },
    { "EBQ" ,	 "A", 	 "D", 	 "2-[(2R)-OXIRAN-2-YL]ETHYLALPHA-D-GLUCOPYRANO" ,   "O5 C1 C2 C3 C4 C5" },
    { "EMP" ,	 "A", 	 "L", 	 "2,4-DIDEOXY-4-(ETHYLAMINO)-3-O-METHYLALPHA-L" ,   "O5 C1 C2 C3 C4 C5" },
    { "EPG" ,	 "A", 	 "D", 	 "2-HYDROXYMETHYL-6-OXIRANYLMETHOXY-TETRAHYDRO-" ,  "O5 C1 C2 C3 C4 C5" },
    { "EQP" ,	 "A", 	 "D", 	 "(4-ACETAMIDO-2,4-DIDEOXY-D-GLYCERO-ALPHA-D-GA" ,  "O6 C2 C3 C4 C5 C6" },
    { "ERE" ,	 "A", 	 "L", 	 "(1R,3S,4R,5S)-3-AMINO-2,3,6-TRIDEOXY-3-METHYL" ,  "O5 C1 C2 C3 C4 C5" },
    { "F1P" ,	 "B", 	 "D", 	 "1-O-PHOSPHONO-BETA-D-FRUCTOPYRANOSE" ,            "O6 C2 C3 C4 C5 C6" },
    { "F1X" ,	 "B", 	 "D", 	 "1-O-PHOSPHONO-BETA-D-FRUCTOFURANOSE" ,            "O5 C2 C3 C4 C5"    },
    { "F6P" ,	 "B", 	 "D", 	 "FRUCTOSE-6-PHOSPHATE" , 	 		    "O5 C2 C3 C4 C5"    },
    { "FBP" ,	 "B", 	 "D", 	 "BETA-FRUCTOSE-1,6-DIPHOSPHATE" ,                  "O5 C2 C3 C4 C5"    },
    { "FCA" ,	 "A", 	 "D", 	 "ALPHA-D-FUCOSE" ,                                 "O5 C1 C2 C3 C4 C5" },
    { "FCB" ,	 "B", 	 "D", 	 "BETA-D-FUCOSE" ,                                  "O5 C1 C2 C3 C4 C5" },
    { "FDP" ,	 "B", 	 "D", 	 "FRUCTOSE-2,6-DIPHOSPHATE" ,                       "O5 C2 C3 C4 C5"    },
    { "FRU" ,	 "B", 	 "D", 	 "FRUCTOSE" ,                                       "O5 C2 C3 C4 C5"    },
    { "FUC" ,	 "A", 	 "L", 	 "ALPHA-L-FUCOSE" ,                                 "O5 C1 C2 C3 C4 C5" },
    { "FUL" ,	 "B", 	 "L", 	 "BETA-L-FUCOSE" ,                                  "O5 C1 C2 C3 C4 C5" },
    { "FUY" ,	 "B", 	 "L", 	 "BETA-L-FUCOSYL-AZIDE" ,                           "O5 C1 C2 C3 C4 C5" },
    { "G0S" ,	 "B", 	 "D", 	 "3-(BETA-D-GALACTOPYRANOSYLTHIO)PROPANOICACID" ,   "O5 C1 C2 C3 C4 C5" },
    { "G16" ,	 "A", 	 "D", 	 "ALPHA-D-GLUCOSE1,6-BISPHOSPHATE" ,                "O5 C1 C2 C3 C4 C5" },
    { "G1P" ,	 "A", 	 "D", 	 "ALPHA-D-GLUCOSE-1-PHOSPHATE" ,                    "O5 C1 C2 C3 C4 C5" },
    { "G2F" ,	 "A", 	 "D", 	 "2-DEOXY-2-FLUORO-ALPHA-D-GLUCOPYRANOSE" ,         "O5 C1 C2 C3 C4 C5" },
    { "G3F" ,	 "B", 	 "D", 	 "3-DEOXY-3-FLUORO-BETA-D-GLUCOPYRANOSE" ,          "O5 C1 C2 C3 C4 C5" },
    { "G4D" ,	 "A", 	 "D", 	 "4-DEOXY-ALPHA-D-GLUCOSE" ,                        "O5 C1 C2 C3 C4 C5" },
    { "G4S" ,	 "B", 	 "D", 	 "4-O-SULFO-BETA-D-GALACTOPYRANOSE" ,               "O5 C1 C2 C3 C4 C5" },
    { "G6P" ,	 "A", 	 "D", 	 "ALPHA-D-GLUCOSE-6-PHOSPHATE" ,                    "O5 C1 C2 C3 C4 C5" },
    { "G6S" ,	 "B", 	 "D", 	 "D-GALACTOSE-6-SULFATE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "G7P" ,	 "B", 	 "D", 	 "6,7-DIDEOXY-7-PHOSPHONO-BETA-D-GLUCO-HEPTOPYR" ,  "O5 C1 C2 C3 C4 C5" },
    { "GAF" ,	 "A", 	 "D", 	 "2-DEOXY-2-FLUORO-ALPHA-D-GALACTOPYRANOSE" ,       "O5 C1 C2 C3 C4 C5" },
    { "GAL" ,	 "B", 	 "D", 	 "BETA-D-GALACTOSE" ,                               "O5 C1 C2 C3 C4 C5" },
    { "GLA" ,	 "A", 	 "D", 	 "ALPHAD-GALACTOSE" ,                               "O5 C1 C2 C3 C4 C5" },
    { "GC4" ,	 "B", 	 "D", 	 "4-DEOXY-D-GLUCURONICACID" ,                       "O5 C1 C2 C3 C4 C5" },
    { "GCD" ,	 "A", 	 "L", 	 "4,5-DEHYDRO-D-GLUCURONICACID" ,                   "O5 C1 C2 C3 C4 C5" },
    { "GCN" ,	 "A", 	 "D", 	 "3-DEOXY-D-GLUCOSAMINE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "GCS" ,	 "B", 	 "D", 	 "D-GLUCOSAMINE" ,                                  "O5 C1 C2 C3 C4 C5" },
    { "GCU" ,	 "A", 	 "D", 	 "D-GLUCURONICACID" ,                               "O5 C1 C2 C3 C4 C5" },
    { "GCV" ,	 "A", 	 "D", 	 "4-O-METHYL-ALPHA-D-GLUCURONICACID" , 	            "O5 C1 C2 C3 C4 C5" },
    { "GCW" ,	 "B", 	 "D", 	 "4-O-METHYL-BETA-D-GLUCURONICACID" ,               "O5 C1 C2 C3 C4 C5" },
    { "GDA" ,	 "B", 	 "D", 	 "4-DEOXY-4-AMINO-BETA-D-GLUCOSE" ,                 "O5 C1 C2 C3 C4 C5" },
    { "GDL" ,	 "N", 	 "D", 	 "2-ACETAMIDO-2-DEOXY-D-GLUCONO-1,5-LACTONE" ,      "O5 C1 C2 C3 C4 C5" },
    { "GFP" ,	 "A", 	 "D", 	 "2-DEOXY-2-FLUORO-ALPHA-D-GLUCOSE-1-PHOSPHATE" ,   "O5 C1 C2 C3 C4 C5" },
    { "GL0" ,	 "B", 	 "D", 	 "BETA-D-GULOPYRANOSE" ,                            "O5 C1 C2 C3 C4 C5" },
    { "GL1" ,	 "A", 	 "D", 	 "1-O-PHOSPHONO-ALPHA-D-GALACTOPYRANOSE" ,          "O5 C1 C2 C3 C4 C5" },
    { "GL2" ,	 "N", 	 "D", 	 "3-AMINO-8,9,10-TRIHYDROXY-7-HYDROXYMETHYL-6-O" ,  "O5 C1 C2 C3 C4 C5" },
    { "GL4" ,	 "N", 	 "D", 	 "8,9,10-TRIHYDROXY-7-HYDROXYMETHYL-2-THIOXO-6-" ,  "O5 C1 C2 C3 C4 C5" },
    { "GL5" ,	 "N", 	 "D", 	 "3,8,9,10-TETRAHYDROXY-7-HYDROXYMETHYL-6-OXA-1" ,  "O5 C1 C2 C3 C4 C5" },
    { "GL6" ,	 "N", 	 "D", 	 "(3,4,5-TRIHYDROXY-6-HYDROXYMETHYL-TETRAHYDRO-" ,  "O5 C1 C2 C3 C4 C5" },
    { "GL7" ,	 "N", 	 "D", 	 "8,9,10-TRIHYDROXY-7-HYDROXYMETHYL-3-METHYL-6-" ,  "O5 C1 C2 C3 C4 C5" },
    { "GL9" ,	 "N", 	 "D", 	 "N-(8,9,10-TRIHYDROXY-7-HYDROXYMETHYL-2,4-DIOX" ,  "O5 C1 C2 C3 C4 C5" },
    { "GLA" ,	 "A", 	 "D", 	 "ALPHAD-GALACTOSE" ,                               "O5 C1 C2 C3 C4 C5" },
    { "GLC" ,	 "A", 	 "D", 	 "ALPHA-D-GLUCOSE" ,                                "O5 C1 C2 C3 C4 C5" },
    { "GLD" ,	 "A", 	 "D", 	 "4,6-DIDEOXYGLUCOSE" ,                             "O5 C1 C2 C3 C4 C5" },
    { "GLF" ,	 "A", 	 "D", 	 "1-FLUORO-ALPHA-1-DEOXY-D-GLUCOSE" ,               "O5 C1 C2 C3 C4 C5" },
    { "GLG" ,	 "A", 	 "D", 	 "ALPHA-D-GLUCOPYRANOSYL-2-CARBOXYLICACIDAMID" ,    "O5 C1 C2 C3 C4 C5" },
    { "GLP" ,	 "A", 	 "D", 	 "GLUCOSAMINE6-PHOSPHATE" ,                         "O5 C1 C2 C3 C4 C5" },
    { "GLS" ,	 "B", 	 "D", 	 "BETA-D-GLUCOPYRANOSESPIROHYDANTOIN" ,             "O5 C1 C2 C3 C4 C5" },
    { "GLT" ,	 "A", 	 "D", 	 "5-DEOXY-5-THIO-ALPHA-D-GLUCOSE" ,                 "O5 C1 C2 C3 C4 C5" },
    { "GMB" ,	 "B", 	 "D", 	 "1,7-DI-O-PHOSPHONO-L-GLYCERO-BETA-D-MANNO-HEP" ,  "O5 C1 C2 C3 C4 C5" },
    { "GMH" ,	 "A", 	 "D", 	 "L-GLYCERO-D-MANNO-HEPTOPYRANOSE" ,                "O5 C1 C2 C3 C4 C5" },
    { "GNA" ,	 "A", 	 "L", 	 "2,4-DEOXY-4-GUANIDINO-5-N-ACETYL-NEURAMINICA" ,   "O6 C2 C3 C4 C5 C6" },
    { "GS1" ,	 "B", 	 "D", 	 "1-THIO-BETA-D-GLUCOPYRANOSE" ,                    "O5 C1 C2 C3 C4 C5" },
    { "GTK" ,	 "B", 	 "D", 	 "BETA-D-GALACTOFURANURONICACID" ,                  "O4 C1 C2 C3 C4"    },
    { "GTM" ,	 "B", 	 "D", 	 "O1-METHYL-4-DEOXY-4-THIO-BETA-D-GLUCOSE" ,        "O5 C1 C2 C3 C4 C5" },
    { "GTR" ,	 "B", 	 "D", 	 "GALACTURONICACID" ,                               "O5 C1 C2 C3 C4 C5" },
    { "GU0" ,	 "A", 	 "L", 	 "2,3,6-TRI-O-SULFONATO-ALPHA-L-GALACTOPYRANOSE" ,  "O5 C1 C2 C3 C4 C5" },
    { "GU3" ,	 "A", 	 "D", 	 "METHYL3-O-METHYL-2,6-DI-O-SULFO-ALPHA-D-GLUC" ,   "O5 C1 C2 C3 C4 C5" },
    { "GU4" ,	 "A", 	 "D", 	 "2,3,4,6-TETRA-O-SULFONATO-ALPHA-D-GLUCOPYRANO" ,  "O5 C1 C2 C3 C4 C5" },
    { "GU5" ,	 "A", 	 "D", 	 "2,3-DI-O-METHYL-6-O-SULFONATO-ALPHA-D-GLUCOPY" ,  "O5 C1 C2 C3 C4 C5" },
    { "GU6" ,	 "A", 	 "D", 	 "2,3,6-TRI-O-SULFONATO-ALPHA-D-GLUCOPYRANOSE" ,    "O5 C1 C2 C3 C4 C5" },
    { "GU8" ,	 "B", 	 "D", 	 "2,3,6-TRI-O-METHYL-BETA-D-GLUCOPYRANOSE" ,        "O5 C1 C2 C3 C4 C5" },
    { "GU9" ,	 "A", 	 "D", 	 "2,3,6-TRI-O-METHYL-ALPHA-D-GLUCOPYRANOSE" ,       "O5 C1 C2 C3 C4 C5" },
    { "GUF" ,	 "B", 	 "D", 	 "2-DEOXY-2-FLUORO-BETA-D-GLUCOPYRANURONICACID" ,   "O5 C1 C2 C3 C4 C5" },
    { "GUP" ,	 "A", 	 "L", 	 "ALPHA-L-GULOPYRANOSIDE" ,                         "O5 C1 C2 C3 C4 C5" },
    { "GUZ" ,	 "A", 	 "D", 	 "2-DEOXY-2-FLUORO-ALPHA-D-GLUCOPYRANURONICACI" ,   "O5 C1 C2 C3 C4 C5" },
    { "GXL" ,	 "A", 	 "L", 	 "ALPHA-L-GALACTOPYRANOSE" ,                        "O5 C1 C2 C3 C4 C5" },
    { "GYP" ,	 "A", 	 "D", 	 "METHYL-ALPHA-D-GLUCOPYRANOSIDE" ,                 "O5 C1 C2 C3 C4 C5" },
    { "H1M" ,	 "A", 	 "D", 	 "METHYL2-DEOXY-2-(2-HYDROXYETHYL)-ALPHA-D-MAN" ,   "O5 C1 C2 C3 C4 C5" },
    { "H2P" ,	 "A", 	 "D", 	 "HEPTULOSE-2-PHOSPHATE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "IDG" ,	 "B", 	 "L", 	 "O-2,6-DIAMINO-2,6-DIDEOXY-BETA-L-IDOPYRANOSE" ,   "O5 C1 C2 C3 C4 C5" },
    { "IDR" ,	 "A", 	 "L", 	 "L-IDURONICACID" ,                                 "O5 C1 C2 C3 C4 C5" },
    { "IDS" ,	 "A", 	 "L", 	 "2-O-SULFO-ALPHA-L-IDOPYRANURONICACID" ,           "O5 C1 C2 C3 C4 C5" },
    { "IMK" ,	 "B", 	 "D", 	 "2-(BETA-D-GLUCOPYRANOSYL)-5-METHYL-1-BENZIMID" ,  "O5 C1 C2 C3 C4 C5" },
    { "IPT" ,	 "B", 	 "D", 	 "ISOPROPYL-1-BETA-D-THIOGALACTOSIDE" ,             "O5 C1 C2 C3 C4 C5" },
    { "IXD" ,	 "B", 	 "D", 	 "4-DEOXY-2-O-SULFO-BETA-D-ERYTHRO-HEX-4-ENOPYR" ,  "O5 C1 C2 C3 C4 C5" },
    { "JHM" ,	 "A", 	 "D", 	 "2-DEOXY-6-O-SULFO-ALPHA-D-ARABINO-HEXOPYRANOS" ,  "O5 C1 C2 C3 C4 C5" },
    { "JZR" ,	 "B", 	 "D", 	 "HEXYLBETA-D-GLUCOPYRANOSIDE" ,                    "O5 C1 C2 C3 C4 C5" },
    { "J78" ,    "B",    "L",    "UNKOWN",                                     "CAA CAF CAE CAD CAC CAB"},
    { "K99" ,	 "B", 	 "L", 	 "(2R,3R,4R,5R,6S)-2,3-DIFLUORO-4,5-DIHYDROXY-6" ,  "O6 C2 C3 C4 C5 C6" },
    { "KDA" ,	 "A", 	 "D", 	 "(3-DEOXY-D-MANNO-OCT-2-ULOSONICACID)-2-O-ALL" ,   "O6 C2 C3 C4 C5 C6" },
    { "KDB" ,	 "A", 	 "D", 	 "3,4,5-TRIDEOXY-ALPHA-D-ERYTHRO-OCT-3-EN-2-ULO" ,  "O6 C2 C3 C4 C5 C6" },
    { "KDE" ,	 "B", 	 "L", 	 "PROP-2-EN-1-YL3-DEOXY-BETA-L-GULO-OCT-2-ULOP" ,   "O6 C2 C3 C4 C5 C6" },
    { "KDO" ,	 "A", 	 "D", 	 "3-DEOXY-D-MANNO-OCT-2-ULOSONICACID" ,             "O6 C2 C3 C4 C5 C6" },
    { "KDR" ,	 "A", 	 "D", 	 "PROP-2-EN-1-YL3-DEOXY-ALPHA-D-MANNO-OCTOS-2-" ,   "O6 C2 C3 C4 C5 C6" },
    { "KME" ,	 "A", 	 "D", 	 "(1E)-PROP-1-EN-1-YL3-DEOXY-7-O-METHYL-ALPHA-" ,   "O6 C2 C3 C4 C5 C6" },
    { "KO1" ,	 "A", 	 "D", 	 "D-GLYCERO-ALPHA-D-TALO-OCT-2-ULOPYRANOSONICA" ,   "O6 C2 C3 C4 C5 C6" },
    { "KO2" ,	 "A", 	 "D", 	 "PROP-2-EN-1-YLD-GLYCERO-ALPHA-D-TALO-OCT-2-U" ,   "O6 C2 C3 C4 C5 C6" },
    { "KOT" ,	 "B", 	 "D", 	 "1-BETA-D-GLUCOPYRANOSYL-4-PHENYL-1H-1,2,3-TRI" ,  "O5 C1 C2 C3 C4 C5" },
    { "L6S" ,	 "A", 	 "L", 	 "6-O-SULFO-ALPHA-L-GALACTOSE" ,                    "O5 C1 C2 C3 C4 C5" },
    { "LDY" ,	 "A", 	 "D", 	 "ALPHA-D-LYXOPYRANOSE" ,                           "O5 C1 C2 C3 C4 C5" },
    { "LFR" ,	 "B", 	 "L", 	 "BETA-L-FRUCTO-FURANOSE" ,                         "O5 C1 C2 C3 C4 C5" },
    { "LGC" ,	 "N", 	 "D", 	 "(3S,4R,5R,6S)-3,4,5-TRIHYDROXY-6-(HYDROXYMETH" ,  "O5 C1 C2 C3 C4 C5" },
    { "LGU" ,	 "A", 	 "L", 	 "ALPHA-L-GULURONATE" ,                             "O5 C1 C2 C3 C4 C5" },
    { "LOG" ,	 "N", 	 "D", 	 "LOGNAC" ,                                         "O5 C1 C2 C3 C4 C5" },
    { "LRH" ,	 "B", 	 "L", 	 "6-DEOXY-BETA-L-FRUCTOFURANOSE" ,                  "O5 C1 C2 C3 C4 C5" },
    { "LXB" ,	 "B", 	 "D", 	 "2-(ACETYLAMINO)-2-DEOXY-BETA-D-GULOPYRANOSE" ,    "O5 C1 C2 C3 C4 C5" },
    { "LXC" ,	 "B", 	 "L", 	 "BETA-L-XYLOPYRANOSE" ,                            "O5 C1 C2 C3 C4 C5" },
    { "LXZ" ,	 "A", 	 "D", 	 "2-(ACETYLAMINO)-2-DEOXY-ALPHA-D-IDOPYRANOSE" ,    "O5 C1 C2 C3 C4 C5" },
    { "LZ0" ,	 "A", 	 "L", 	 "[1-(2-OXOETHYL)-1H-1,2,3-TRIAZOL-5-YL]METHYL" ,   "O5 C1 C2 C3 C4 C5" },
    { "M07" ,	 "N", 	 "D", 	 "(5R,7R,8S,9S,10R)-7-(HYDROXYMETHYL)-3-(4-METH" ,  "O5 C1 C2 C3 C4 C5" },
    { "M08" ,	 "N", 	 "D", 	 "(5R,7R,8S,9S,10R)-7-(HYDROXYMETHYL)-3-PHENYL-" ,  "O5 C1 C2 C3 C4 C5" },
    { "M09" ,	 "N", 	 "D", 	 "(3S,5R,7R,8S,9S,10R)-7-(HYDROXYMETHYL)-3-(4-N" ,  "O5 C1 C2 C3 C4 C5" },
    { "M1P" ,	 "A", 	 "D", 	 "ALPHA-D-MANNOSE1-PHOSPHATE" ,                     "O5 C1 C2 C3 C4 C5" },
    { "M6D" ,	 "B", 	 "D", 	 "6-O-PHOSPHONO-BETA-D-MANNOPYRANOSE" ,             "O5 C1 C2 C3 C4 C5" },
    { "M6P" ,	 "A", 	 "D", 	 "ALPHA-D-MANNOSE-6-PHOSPHATE" ,                    "O5 C1 C2 C3 C4 C5" },
    { "M7P" ,	 "B", 	 "D", 	 "D-GLYCERO-D-MANNOPYRANOSE-7-PHOSPHATE" ,          "O5 C1 C2 C3 C4 C5" },
    { "M8C" ,	 "A", 	 "D", 	 "METHYLALPHA-D-GALACTOPYRANURONATE" ,              "O5 C1 C2 C3 C4 C5" },
    { "MA1" ,	 "A", 	 "D", 	 "1,4-DITHIO-ALPHA-D-GLUCOPYRANOSE" ,               "O5 C1 C2 C3 C4 C5" },
    { "MA2" ,	 "A", 	 "D", 	 "4-S-METHYL-4-THIO-ALPHA-D-GLUCOPYRANOSE" ,        "O5 C1 C2 C3 C4 C5" },
    { "MA3" ,	 "A", 	 "D", 	 "O1-METHYL-4-DEOXY-4-THIO-ALPHA-D-GLUCOSE" ,       "O5 C1 C2 C3 C4 C5" },
    { "MAG" ,	 "B", 	 "D", 	 "BETA-METHYL-N-ACETYL-D-GLUCOSAMINE" ,             "O5 C1 C2 C3 C4 C5" },
    { "MAN" ,	 "A", 	 "D", 	 "ALPHA-D-MANNOSE" ,                                "O5 C1 C2 C3 C4 C5" },
    { "MAT" ,	 "A", 	 "L", 	 "2,4-DIDEOXY-4-[2-(PROPYL)AMINO]-3-O-METHYLAL" ,   "O5 C1 C2 C3 C4 C5" },
    { "MAV" ,	 "A", 	 "D", 	 "ALPHA-D-MANNOPYRANURONICACID" ,                   "O5 C1 C2 C3 C4 C5" },
    { "MAW" ,	 "A", 	 "L", 	 "4-DEOXY-ALPHA-L-ERYTHRO-HEX-4-ENOPYRANURONIC" ,   "O5 C1 C2 C3 C4 C5" },
    { "MBF" ,	 "B", 	 "D", 	 "2-DEOXY-2-FLUORO-BETA-D-MANNOSE" ,                "O5 C1 C2 C3 C4 C5" },
    { "MBG" ,	 "B", 	 "D", 	 "METHYL-BETA-GALACTOSE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "MDA" ,	 "B", 	 "D", 	 "2,6-DIDEOXY-3C-METHYL-D-RIBOPYRANOSIDE" ,         "O5 C1 C2 C3 C4 C5" },
    { "MDP" ,	 "A", 	 "D", 	 "N-CARBOXY-N-METHYL-MURAMICACID" ,                 "O5 C1 C2 C3 C4 C5" },
    { "MFB" ,	 "B", 	 "L", 	 "BETA-L-METHYL-FUCOSE" ,                           "O5 C1 C2 C3 C4 C5" },
    { "MFU" ,	 "A", 	 "L", 	 "ALPHA-L-METHYL-FUCOSE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "MGC" ,	 "A", 	 "D", 	 "ALPHA-METHYL-N-ACETYL-D-GALACTOSAMINE" ,          "O5 C1 C2 C3 C4 C5" },
    { "MGL" ,	 "B", 	 "D", 	 "O1-METHYL-GLUCOSE" ,                              "O5 C1 C2 C3 C4 C5" },
    { "MGS" ,	 "A", 	 "D", 	 "1,2-O-DIMETHYL-4-[2,4-DIHYDROXY-BUTYRAMIDO]-4" ,  "O5 C1 C2 C3 C4 C5" },
    { "MMA" ,	 "A", 	 "D", 	 "O1-METHYL-MANNOSE" ,                              "O5 C1 C2 C3 C4 C5" },
    { "MN0" ,	 "A", 	 "D", 	 "METHYL3,5-DIDEOXY-5-[(HYDROXYACETYL)AMINO]-D" ,   "O6 C2 C3 C4 C5 C6" },
    { "MNA" ,	 "A", 	 "D", 	 "2-O-METHYL-5-N-ACETYL-ALPHA-D-NEURAMINICACI" ,    "O6 C2 C3 C4 C5 C6" },
    { "MQT" ,	 "B", 	 "D", 	 "METHYL2-O-ACETYL-3-O-(4-METHYLBENZOYL)-BETA-" ,   "O5 C1 C2 C3 C4 C5" },
    { "MRP" ,	 "A", 	 "L", 	 "3-O-METHYL-ALPHA-L-RHAMNOPYRANOSIDE" ,            "O5 C1 C2 C3 C4 C5" },
    { "MUB" ,	 "A", 	 "D", 	 "N-ACETYLMURAMICACID" ,                            "O5 C1 C2 C3 C4 C5" },
    { "MUR" ,	 "B", 	 "D", 	 "MURAMICACID" ,                                    "O5 C1 C2 C3 C4 C5" },
    { "MXZ" ,	 "A", 	 "L", 	 "6-DEOXY-2-O-METHYL-ALPHA-L-GALACTOPYRANOSE" ,     "O5 C1 C2 C3 C4 C5" },
    { "N1L" ,	 "B", 	 "D", 	 "2-AMINO-2-DEOXY-BETA-D-GLUCOPYRANURONICACID" ,    "O5 C1 C2 C3 C4 C5" },
    { "NA1" ,	 "A", 	 "D", 	 "METHYLN-ACETYLALLOSAMINE" ,                       "O5 C1 C2 C3 C4 C5" },
    { "NAA" ,	 "B", 	 "D", 	 "N-ACETYL-D-ALLOSAMINE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "NAG" ,	 "B", 	 "D", 	 "N-ACETYL-D-GLUCOSAMINE" ,                         "O5 C1 C2 C3 C4 C5" },
    { "NBG" ,	 "B", 	 "D", 	 "1-N-ACETYL-BETA-D-GLUCOSAMINE" ,                  "O5 C1 C2 C3 C4 C5" },
    { "NDG" ,	 "A", 	 "D", 	 "2-(ACETYLAMINO)-2-DEOXY-A-D-GLUCOPYRANOSE" ,      "O C1 C2 C3 C4 C5"  },
    { "NG1" ,	 "A", 	 "D", 	 "N-ACETYL-ALPHA-D-GALACTOSAMINE1-PHOSPHATE" ,      "O5 C1 C2 C3 C4 C5" },
    { "NGA" ,	 "B", 	 "D", 	 "N-ACETYL-D-GALACTOSAMINE" , 	 		    "O5 C1 C2 C3 C4 C5" },
    { "NGZ" ,	 "A", 	 "L", 	 "2-(ACETYLAMINO)-2-DEOXY-ALPHA-L-GLUCOPYRANOSE" ,  "O5 C1 C2 C3 C4 C5" },
    { "NNG" ,	 "A", 	 "D", 	 "2-DEOXY-2-{[(S)-HYDROXY(METHYL)PHOSPHORYL]AMI" ,  "O5 C1 C2 C3 C4 C5" },
    { "NTF" ,	 "B", 	 "D", 	 "N-TRIFLURO-ACETYL-BETA-D-GLUCOPYRANOSYLAMINE" ,   "O5 C1 C2 C3 C4 C5" },
    { "NXD" ,	 "A", 	 "D", 	 "METHYL5-(ACETYLAMINO)-9-{[AMINO(OXO)ACETYL]A" ,   "O6 C2 C3 C4 C5 C6" },
    { "OAK" ,	 "B", 	 "D", 	 "N-(PHENYLCARBONYL)-BETA-D-GLUCOPYRANOSYLAMINE" ,  "O5 C1 C2 C3 C4 C5" },
    { "OI7" ,	 "B", 	 "D", 	 "1,7-DI-O-PHOSPHONO-BETA-D-ALTRO-HEPT-2-ULOFUR" ,  "O5 C2 C3 C4 C5"    },
    { "OPM" ,	 "A", 	 "D", 	 "O1-PENTYL-MANNOSE" ,                              "O5 C1 C2 C3 C4 C5" },
    { "OTG" ,	 "A", 	 "D", 	 "ORTHO-TOLUOYLGLUCOSAMINE" ,                       "O5 C1 C2 C3 C4 C5" },
    { "OX2" ,	 "B", 	 "D", 	 "2-(BETA-D-GLUCOPYRANOSYL)-5-METHYL-1,3,4-OXAD" ,  "O5 C1 C2 C3 C4 C5" },
    { "P6P" ,	 "A", 	 "D", 	 "6-O-PHOSPHONO-ALPHA-D-FRUCTOFURANOSE" ,           "O5 C2 C3 C4 C5"    },
    { "PA1" ,	 "A", 	 "D", 	 "2-AMINO-2-DEOXY-ALPHA-D-GLUCOPYRANOSE" ,          "O5 C1 C2 C3 C4 C5" },
    { "PDX" ,	 "A", 	 "D", 	 "2,3-DI-O-SULFO-ALPHA-D-GLUCOPYRANOSE" ,           "O5 C1 C2 C3 C4 C5" },
    { "PH5" ,	 "A", 	 "D", 	 "2-PHENYL-PROP5AC" ,                               "O6 C2 C3 C4 C5 C6" },
    { "PNA" ,	 "A", 	 "D", 	 "4'-NITROPHENYL-ALPHA-D-MANNOPYRANOSIDE" ,         "O5 C1 C2 C3 C4 C5" },
    { "PNG" ,	 "A", 	 "D", 	 "4'-NITROPHENYL-ALPHA-D-GLUCOPYRANOSIDE" ,         "O5 C1 C2 C3 C4 C5" },
    { "PNJ" ,	 "B", 	 "D", 	 "PNP-BETA-D-GLUCOSAMINE" ,                         "O5 C1 C2 C3 C4 C5" },
    { "PNW" ,	 "B", 	 "D", 	 "4-NITROPHENYLBETA-D-GLUCOPYRANOSIDE" ,            "O5 C1 C2 C3 C4 C5" },
    { "PPC" ,	 "A", 	 "D", 	 "5-PHOSPHORIBOSYL-1-(BETA-METHYLENE)PYROPHOSP" ,   "O4 C1 C2 C3 C4"    },
    { "PRP" ,	 "A", 	 "D", 	 "ALPHA-PHOSPHORIBOSYLPYROPHOSPHORICACID" ,         "O4 C1 C2 C3 C4"    },
    { "PSG" ,	 "B", 	 "D", 	 "PARA-NITROPHENYL1-THIO-BETA-D-GLUCOPYRANOSID" ,   "O5 C1 C2 C3 C4 C5" },
    { "PSV" ,	 "A", 	 "D", 	 "ALPHA-D-PSICOFURANOSE" ,                          "O5 C1 C2 C3 C4 C5" },
    { "RAM" ,	 "A", 	 "L", 	 "ALPHA-L-RHAMNOSE" ,                               "O5 C1 C2 C3 C4 C5" },
    { "RAO" ,	 "A", 	 "L", 	 "1-O-METHYL-ALPHA-RHAMNOSE" ,                      "O5 C1 C2 C3 C4 C5" },
    { "RER" ,	 "A", 	 "L", 	 "(1R,3S,4S,5S)-3-AMINO-2,3,6-TRIDEOXY-3-METHYL" ,  "O5 C1 C2 C3 C4 C5" },
    { "RGG" ,	 "B", 	 "D", 	 "(2R)-2,3-DIHYDROXYPROPYLBETA-D-GALACTOPYRANO" ,   "O5 C1 C2 C3 C4 C5" },
    { "RHC" ,	 "B", 	 "D", 	 "5-(3-AMINO-4,4-DIHYROXY-BUTYLSULFANYLMETHYL)-" ,  "O4 C1 C2 C3 C4 C5" },
    { "RI2" ,	 "A", 	 "D", 	 "1,5-DI-O-PHOSPHONO-ALPHA-D-RIBOFURANOSE" ,        "O4 C1 C2 C3 C4"    },
    { "RIP" ,	 "B", 	 "D", 	 "RIBOSE(PYRANOSEFORM)" ,                           "O5 C1 C2 C3 C4 C5" },
    { "RM4" ,	 "B", 	 "L", 	 "6-DEOXY-BETA-L-MANNOPYRANOSE" ,                   "O5 C1 C2 C3 C4 C5" },
    { "RST" ,	 "A", 	 "L", 	 "RISTOSAMINE" ,                                    "O5 C1 C2 C3 C4 C5" },
    { "RUG" ,	 "B", 	 "D", 	 "1-BETA-D-GLUCOPYRANOSYL-4-(HYDROXYMETHYL)-1H-" ,  "O5 C1 C2 C3 C4 C5" },
    { "S06" ,	 "N", 	 "D", 	 "(3S,5R,7R,8S,9S,10R)-7-(HYDROXYMETHYL)-3-(2-N" ,  "O5 C1 C2 C3 C4 C5" },
    { "S13" ,	 "N", 	 "D", 	 "(3S,5R,7R,8S,9S,10R)-7-(HYDROXYMETHYL)-3-(4-M" ,  "O5 C1 C2 C3 C4 C5" },
    { "SFU" ,	 "A", 	 "L", 	 "METHYL6-DEOXY-1-SELENO-ALPHA-L-GALACTOPYRANO" ,   "O5 C1 C2 C3 C4 C5" },
    { "SG4" ,	 "A", 	 "D", 	 "3,4-DI-O-ACETYL-6-O-SULFAMOYL-ALPHA-D-GLUCOPY" ,  "O5 C1 C2 C3 C4 C5" },
    { "SGA" ,	 "B", 	 "D", 	 "O3-SULFONYLGALACTOSE" ,                           "O5 C1 C2 C3 C4 C5" },
    { "SGC" ,	 "B", 	 "D", 	 "4-DEOXY-4-THIO-BETA-D-GLUCOPYRANOSE" ,            "O5 C1 C2 C3 C4 C5" },
    { "SGN" ,	 "A", 	 "D", 	 "N,O6-DISULFO-GLUCOSAMINE" ,                       "O5 C1 C2 C3 C4 C5" },
    { "SHG" ,	 "B", 	 "D", 	 "2-DEOXY-2-FLUORO-BETA-D-GLUCOPYRANOSE" ,          "O5 C1 C2 C3 C4 C5" },
    { "SIA" ,	 "A", 	 "L", 	 "O-SIALIC ACID" ,                                  "O6 C2 C3 C4 C5 C6" },
    { "SID" ,	 "A", 	 "D", 	 "METHYL9-S-ACETYL-5-(ACETYLAMINO)-3,5-DIDEOXY" ,   "O6 C2 C3 C4 C5 C6" },
    { "SLB" ,	 "B", 	 "D", 	 "5-N-ACETYL-BETA-D-NEURAMINICACID" ,               "O6 C2 C3 C4 C5 C6" },
    { "SLM" ,	 "A", 	 "L", 	 "SIALYLAMIDE" ,                                    "O6 C2 C3 C4 C5 C6" },
    { "SN5" ,	 "B", 	 "D", 	 "2-DEOXY-2-(ETHANETHIOYLAMINO)-BETA-D-GLUCOPYR" ,  "O5 C1 C2 C3 C4 C5" },
    { "SNG" ,	 "B", 	 "D", 	 "METHYL2-ACETAMIDO-1,2-DIDEOXY-1-SELENO-BETA-" ,   "O5 C1 C2 C3 C4 C5" },
    { "SOE" ,	 "A", 	 "L", 	 "ALPHA-L-SORBOPYRANOSE" ,                          "O6 C2 C3 C4 C5 C6" },
    { "SSG" ,	 "B", 	 "D", 	 "1,4-DEOXY-1,4-DITHIO-BETA-D-GLUCOPYRANOSE" ,      "O5 C1 C2 C3 C4 C5" },
    { "STZ" ,	 "B", 	 "D", 	 "STREPTOZOTOCIN" ,                                 "O5 C1 C2 C3 C4 C5" },
    { "SUS" ,	 "A", 	 "D", 	 "2-DEOXY-3,6-DI-O-SULFO-2-(SULFOAMINO)-ALPHA-D" ,  "O5 C1 C2 C3 C4 C5" },
    { "TA6" ,	 "B", 	 "D", 	 "6-O-PHOSPHONO-BETA-D-TAGATOFURANOSE" ,            "O5 C1 C2 C3 C4 C5" },
    { "TGA" ,	 "B", 	 "D", 	 "METHANETHIOSULFONYL-GALACTOSIDE" ,                "O5 C1 C2 C3 C4 C5" },
    { "TMR" ,	 "B", 	 "D", 	 "2,6-DIDEOXY-4-THIOMETHYL-BETA-D-RIBOHEXOPYRAN" ,  "O5 C1 C2 C3 C4 C5" },
    { "TMX" ,	 "B", 	 "D", 	 "2-DEOXY-2-(TRIMETHYLAMMONIO)-BETA-D-GLUCOPYRA" ,  "O5 C1 C2 C3 C4 C5" },
    { "TNR" ,	 "A", 	 "D", 	 "O-(2-ACETAMIDO-2-DEOXY-ALPHA-D-GALACTOPYRANOS" ,  "O5 C1 C2 C3 C4 C5" },
    { "TOA" ,	 "A", 	 "D", 	 "3-DEOXY-3-AMINOGLUCOSE" ,                         "O5 C1 C2 C3 C4 C5" },
    { "TOC" ,	 "A", 	 "D", 	 "2,3,6-TRIDEOXY-2,6-DIAMINOGLUCOSE" , 	            "O5 C1 C2 C3 C4 C5" },
    { "TYV" ,	 "A", 	 "D", 	 "TYVELOSE" ,                                       "O5 C1 C2 C3 C4 C5" },
    { "UAP" ,	 "A", 	 "L", 	 "1,4-DIDEOXY-5-DEHYDRO-O2-SULFO-GLUCURONICACI" ,   "O5 C1 C2 C3 C4 C5" },
    { "VG1" ,	 "A", 	 "D", 	 "ALPHA-D-GLUCOSE-1-PHOSPHATE-6-VANADATE" ,         "O5 C1 C2 C3 C4 C5" },
    { "X1P" ,	 "A", 	 "D", 	 "1-O-PHOSPHONO-ALPHA-D-XYLOPYRANOSE" ,             "O5 C1 C2 C3 C4 C5" },
    { "X2F" ,	 "A", 	 "D", 	 "2-DEOXY-2-FLUOROXYLOPYRANOSE" ,                   "O5 C1 C2 C3 C4 C5" },
    { "XYF" ,	 "B", 	 "D", 	 "5(R)-5-FLUORO-BETA-D-XYLOPYRANOSYL-ENZYMEINT" ,   "O5 C1 C2 C3 C4 C5" },
    { "XYP" ,	 "B",	 "D",	 "BETA-D-XYLOPYRANOSE" ,                       "O5B C1B C2B C3B C4B C5B"},
    { "XYS" ,	 "A", 	 "D", 	 "XYLOPYRANOSE" ,                                   "O5 C1 C2 C3 C4 C5" },
    { "YX0" ,	 "A", 	 "L", 	 "[(3E)-3-(1-HYDROXYETHYLIDENE)-2,3-DIHYDROISOX" ,  "O5 C1 C2 C3 C4 C5" },
    { "YX1" ,	 "B", 	 "D", 	 "2-DEOXY-2-{[(2-HYDROXY-1-METHYLHYDRAZINO)CARB" ,  "O5 C1 C2 C3 C4 C5" },
    { "  A" ,	 "B",	 "D", 	 "ADENOSINE-5'-MONOPHOSPHATE" ,                   "O4' C1' C2' C3' C4'" },  // auxiliary support for RNA/DNA
    { "  U" ,    "B",    "D",    "URIDINE-5'-MONOPHOSPHATE" ,                     "O4' C1' C2' C3' C4'" },  // riboses/deoxyriboses
    { "  G" ,    "B",    "D",    "GUANOSINE-5'-MONOPHOSPHATE" ,                   "O4' C1' C2' C3' C4'" },
    { "  C" ,    "B",    "D",    "CYTIDINE-5'-MONOPHOSPHATE" ,                    "O4' C1' C2' C3' C4'" },
    { " DA" ,	 "B",	 "D", 	 "2'-DEOXYADENOSINE-5'-MONOPHOSPHATE" ,           "O4' C1' C2' C3' C4'" },
    { " DG" ,    "B",    "D",    "2'-DEOXYGUANOSINE-5'-MONOPHOSPHATE" ,           "O4' C1' C2' C3' C4'" },
    { " DC" ,    "B",    "D",    "2'-DEOXYCYTIDINE-5'-MONOPHOSPHATE" ,            "O4' C1' C2' C3' C4'" },
    { " DT" ,    "B",    "D",    "2'-DEOXYTHYMIDINE-5'-MONOPHOSPHATE" ,           "O4' C1' C2' C3' C4'" } } ;

    const int sugar_database_size = sizeof( sugar_database ) / sizeof( sugar_database[0] );

} // namespace data

} // namespace clipper
