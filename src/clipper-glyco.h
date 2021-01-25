/*! \file clipper-glyco.h
  Header file for handling sugar data */

// clipper-glyco.h: a set of tools for handling sugars
// version  0.9.1
// 2013 Jon Agirre & Kevin Cowtan, The University of York
// mailto: jon.agirre@york.ac.uk
// mailto: kevin.cowtan@york.ac.uk
//
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

#ifndef CLIPPER_GLYCO_H_INCLUDED
#define CLIPPER_GLYCO_H_INCLUDED

#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <clipper/clipper.h>
#include <clipper/clipper-mmdb.h>
#include <clipper/clipper-minimol.h>
#include "clipper-glyco_data.h"
#include <clipper/minimol/minimol_utils.h>

#ifdef _WIN32
    #undef snprintf
#endif
#include <nlohmann/json.hpp>

inline bool altconf_compatible ( char m1, char m2 )
{
    if (( m1 == 'A' && m2 == 'B') || ( m1 == 'B' && m2 == 'A'))
        return false;
    else
        return true;
}
//!< false if one is from conf A and the other is from conf B


inline clipper::Vec3<clipper::ftype> find_aromatic_plane ( clipper::MMonomer mmon )
{
    clipper::Vec3<clipper::ftype> result (0.0, 0.0, 0.0);

    if ( mmon.type() == "TRP" )
    {
        clipper::Vec3<clipper::ftype> vec1 ( mmon.find ( "CD1" ).coord_orth().x() - mmon.find ( "CD2" ).coord_orth().x(),
                                             mmon.find ( "CD1" ).coord_orth().y() - mmon.find ( "CD2" ).coord_orth().y(),
                                             mmon.find ( "CD1" ).coord_orth().z() - mmon.find ( "CD2" ).coord_orth().z());

        clipper::Vec3<clipper::ftype> vec2 ( mmon.find ( "CE2" ).coord_orth().x() - mmon.find ( "CD2" ).coord_orth().x(),
                                             mmon.find ( "CE2" ).coord_orth().y() - mmon.find ( "CD2" ).coord_orth().y(),
                                             mmon.find ( "CE2" ).coord_orth().z() - mmon.find ( "CD2" ).coord_orth().z());
        result = clipper::Vec3<clipper::ftype>::cross ( vec1, vec2 );
        return result.unit();

    }
    else if ( mmon.type() == "TYR" )
    {
        clipper::Vec3<clipper::ftype> vec2 ( mmon.find ( "CE1" ).coord_orth().x() - mmon.find ( "CG" ).coord_orth().x(),
                                             mmon.find ( "CE1" ).coord_orth().y() - mmon.find ( "CG" ).coord_orth().y(),
                                             mmon.find ( "CE1" ).coord_orth().z() - mmon.find ( "CG" ).coord_orth().z());

        clipper::Vec3<clipper::ftype> vec1 ( mmon.find ( "CE2" ).coord_orth().x() - mmon.find ( "CG" ).coord_orth().x(),
                                             mmon.find ( "CE2" ).coord_orth().y() - mmon.find ( "CG" ).coord_orth().y(),
                                             mmon.find ( "CE2" ).coord_orth().z() - mmon.find ( "CG" ).coord_orth().z());
        result = clipper::Vec3<clipper::ftype>::cross ( vec1, vec2 );
        return result.unit();
    }
    else if ( mmon.type() == "PHE" )
    {
        clipper::Vec3<clipper::ftype> vec2 ( mmon.find ( "CE1" ).coord_orth().x() - mmon.find ( "CG" ).coord_orth().x(),
                                             mmon.find ( "CE1" ).coord_orth().y() - mmon.find ( "CG" ).coord_orth().y(),
                                             mmon.find ( "CE1" ).coord_orth().z() - mmon.find ( "CG" ).coord_orth().z());

        clipper::Vec3<clipper::ftype> vec1 ( mmon.find ( "CE2" ).coord_orth().x() - mmon.find ( "CG" ).coord_orth().x(),
                                             mmon.find ( "CE2" ).coord_orth().y() - mmon.find ( "CG" ).coord_orth().y(),
                                             mmon.find ( "CE2" ).coord_orth().z() - mmon.find ( "CG" ).coord_orth().z());
        result = clipper::Vec3<clipper::ftype>::cross ( vec1, vec2 );
        return result.unit();
    }
    else if ( mmon.type() == "HIS" )
    {
        clipper::Vec3<clipper::ftype> vec2 ( mmon.find ( "CE1" ).coord_orth().x() - mmon.find ( "CG" ).coord_orth().x(),
                                             mmon.find ( "CE1" ).coord_orth().y() - mmon.find ( "CG" ).coord_orth().y(),
                                             mmon.find ( "CE1" ).coord_orth().z() - mmon.find ( "CG" ).coord_orth().z());

        clipper::Vec3<clipper::ftype> vec1 ( mmon.find ( "NE2" ).coord_orth().x() - mmon.find ( "CG" ).coord_orth().x(),
                                             mmon.find ( "NE2" ).coord_orth().y() - mmon.find ( "CG" ).coord_orth().y(),
                                             mmon.find ( "NE2" ).coord_orth().z() - mmon.find ( "CG" ).coord_orth().z());
        result = clipper::Vec3<clipper::ftype>::cross ( vec1, vec2 );
        return result.unit();
    }
    return result;
}


inline clipper::ftype get_angle ( clipper::Vec3<clipper::ftype> vec1, clipper::Vec3<clipper::ftype> vec2 )
{
    clipper::ftype angle = acos ( clipper::Vec3<clipper::ftype>::dot ( vec1, vec2 ) /
                                  (sqrt ( pow(vec1[0],2) + pow(vec1[1],2) + pow(vec1[2],2)) *
                                   sqrt ( pow(vec2[0],2) + pow(vec2[1],2) + pow(vec2[2],2))) ) ;

    return angle;
}

//! Clipper extensions for handling sugars. All code within this namespace shall respect Clipper conventions 
namespace clipper
{

    //! A class for handling monosaccharides in the pyranose or furanose forms
    /*! The MiniMol Sugar object is a derivation of clipper::MMonomer,
        and holds information specific to 5- and 6-membered cyclic sugars

        Upon creation, the input MMonomer is subjected to a sanity check
        against a database built into this library, which contains information
        like anomer, handedness, complete name and a list of the atoms integrating
        the sugar ring. Cremer-Pople parameters and conformation codes are provided
        for analysis and validation purposes. Information about connectivity is
        readily available right after creation too.

        It is strongly suggested that an MAtomNonBond object is supplied to the
        constructor, in order to avoid the creation (time expensive) of an object
        for each MSugar object.
    */

    class MSugar : public clipper::MMonomer
    {

        public:

            MSugar ();
            MSugar ( const clipper::MiniMol& mmol, const clipper::MMonomer& mmon, char alt_conf = ' ' ); //!< default constructor
            MSugar ( const clipper::MiniMol& mmol, const clipper::MMonomer& mmon, const clipper::MAtomNonBond& manb, char alt_conf = ' ' );
            MSugar ( const clipper::MiniMol& mmol, const clipper::MMonomer& mmon, const clipper::MAtomNonBond& manb, clipper::data::sugar_database_entry& validation_data, char alt_conf = ' '  );
            //!< provide pre-calculated (time expensive) MAtomNonBond object. This object will tipically be re-used for many MSugar objects

            const bool operator== ( const clipper::MSugar& m2 ) const { return ( this->id() == m2.id() ); }

            const clipper::String conformation_name() const { return clipper::data::conformational_landscape[sugar_conformation]; }
            //!< get a fixed three-character code for the conformation

            const clipper::String conformation_name_iupac() const { return clipper::data::iupac_conformational_landscape[sugar_conformation]; }
            //!< get HTML-formatted, iupac-compliant codes describing the conformation of the sugar ring

            const clipper::ftype puckering_amplitude() const { return sugar_cremer_pople_params[0]; }
            //!< convenience function for getting the puckering amplitude (in Angstroems) of the sugar ring

            int conformation_code() const { return sugar_conformation; }
            //!< function for working with a numerical denomination of sugar conformation

            const clipper::MAtom& anomeric_carbon() const { return this->sugar_anomeric_carbon; }
            //!< returns the anomeric carbon

            const clipper::MAtom& anomeric_substituent() const { return this->sugar_anomeric_substituent; }
            //!< returns the anomeric substituent

            const clipper::MAtom& configurational_carbon() const { return this->sugar_configurational_carbon; }
            //!< returns the configurational carbon

            const clipper::MAtom& configurational_substituent() const { return this->sugar_configurational_substituent; }
            //!< returns the configurational substituent

            clipper::String anomer() const { return sugar_anomer; }
            //!< convenience function to get the anomer type, e.g. "alpha"

            clipper::String handedness() const { return sugar_handedness; }
            //!< get the sugar's handedness: "D" for dextro, "L" for laevo, "N" for undetermined and "X" for unsupported (missing from the clipper::data sugar database)

            clipper::String type_of_sugar() const { return sugar_denomination; }
            //!< returns a string containing an anomer-handedness-type denomination, e.g. "beta-D-aldopyranose"

            int ring_cardinality() const { return sugar_ring_elements.size(); }
            //!< get the number of atoms forming the main sugar ring

            std::vector<clipper::MAtom> ring_members() const { return sugar_ring_elements; }
            //!< returns an standard vector containing those MAtom's forming the ring

            const clipper::Coord_orth& ring_centre() const { return this->sugar_centre; }
            //!< get the ring's centre (original coordinates)

            const clipper::Vec3<ftype>& ring_mean_plane() const { return sugar_mean_plane; }
            //!< get the vector normal to the sugar ring's mean plane, with origin in ring_centre()

            std::vector<ftype> cremer_pople_params() const { return sugar_cremer_pople_params; }
            //!< returns Cremer-Pople parameters (Cremer and Pople, 1975) in the form { Q , Angle1, ... }

            int potential_linkages() const { return sugar_linked_to.size(); }
            //!<  returns the number of potential linkages to other sugars

            std::vector < std::pair< clipper::MAtomIndexSymmetry, clipper::ftype > > get_stacked_residues ( ) const ;
            //!< returns chain and monomer for stacked residues (restricted to TRP, TYR, PHE, HIS)

            bool is_sane() const { return sugar_sane; }
            //!< checks it against the internal clipper::data sugar database for correct anomer, handedness and ring members

            clipper::String full_name() const { return sugar_name; }

            clipper::String full_type() const { return sugar_type; }

            std::vector<clipper::ftype> ring_angles() const { return sugar_ring_angles; }

            std::vector<clipper::ftype> ring_bonds() const { return sugar_ring_bonds; }

            std::vector<clipper::ftype> ring_torsions() const { return sugar_ring_torsion; }

            clipper::ftype ring_bond_rmsd() const { return sugar_ring_bond_rmsd; }

            clipper::ftype ring_angle_rmsd() const { return sugar_ring_angle_rmsd; }

            clipper::ftype get_bfactor() const { return sugar_bfactor; }

            bool in_database(clipper::String name) const { return sugar_found_db; }

            bool is_supported() const { return sugar_supported; }

            static bool search_database(clipper::String name)
            {
                for (int i = 0; i < clipper::data::sugar_database_size ; i++)
                    if (name.trim() == clipper::data::sugar_database[i].name_short.trim())
                        return true;

                return false;
            } //!< returns true if found

            bool ok_with_ring() const { return sugar_diag_ring; }

            bool ok_with_bonds_rmsd() const { return sugar_diag_bonds_rmsd; }  // compare with database, if not found report dev from ideal

            bool ok_with_angles_rmsd() const { return sugar_diag_angles_rmsd; } // same

            bool ok_with_anomer() const { return sugar_diag_anomer; }

            bool ok_with_chirality() const { return sugar_diag_chirality; }

            bool ok_with_conformation() const { return sugar_diag_conformation; }

            bool ok_with_puckering() const { return sugar_diag_puckering; }

            clipper::ftype get_rscc() const { return sugar_rscc; }

            clipper::ftype get_accum() const { return sugar_accum; }

            bool get_occupancy_check() const { return sugar_occupancy_check; }

            void set_rscc ( clipper::ftype rscc_in ) { sugar_rscc = rscc_in; }

            void set_accum_score ( clipper::ftype accum_in ) { sugar_accum = accum_in; }

            void set_occupancy_check ( bool occupancy_check_in ) { sugar_occupancy_check = occupancy_check_in; }

            clipper::String get_diagnostic() const { return sugar_diagnostic; }

            void set_diagnostic ( clipper::String message ) { sugar_diagnostic = message; }

            clipper::String get_context ( ) const { return sugar_context; }

            void set_context ( clipper::String context ) { this->sugar_context = context; }

        private:

            typedef std::vector< std::pair< clipper::MAtom, clipper::MAtom > > visited_arcs;
            typedef std::pair< std::pair<clipper::MAtom, clipper::MAtom>, std::pair<clipper::MAtom, clipper::MAtom > > stereochemistry_pairs;

            // int codes for conformations, types and anomers

            static const int conf_pyranose_4C1 = 1;  static const int conf_pyranose_1C4 = 2;  static const int conf_pyranose_3OB = 3;  static const int conf_pyranose_B25 = 4;
            static const int conf_pyranose_14B = 5;  static const int conf_pyranose_B3O = 6;  static const int conf_pyranose_25B = 7;  static const int conf_pyranose_B14 = 8;
            static const int conf_pyranose_OE  = 9;  static const int conf_pyranose_E5 = 10;  static const int conf_pyranose_4E = 11;  static const int conf_pyranose_E3 = 12;
            static const int conf_pyranose_2E = 13;  static const int conf_pyranose_E1 = 14;  static const int conf_pyranose_3E = 15;  static const int conf_pyranose_E2 = 16;
            static const int conf_pyranose_1E = 17;  static const int conf_pyranose_EO = 18;  static const int conf_pyranose_5E = 19;  static const int conf_pyranose_E4 = 20;
            static const int conf_pyranose_OH5 =21;  static const int conf_pyranose_4H5 = 22; static const int conf_pyranose_4H3 = 23; static const int conf_pyranose_2H3 = 24;
            static const int conf_pyranose_2H1 =25;  static const int conf_pyranose_OH1 = 26; static const int conf_pyranose_3H2 = 27; static const int conf_pyranose_1H2 = 28;
            static const int conf_pyranose_1HO =29;  static const int conf_pyranose_5HO = 30; static const int conf_pyranose_5H4 = 31; static const int conf_pyranose_3H4 = 32;
            static const int conf_pyranose_OS2 = 33; static const int conf_pyranose_1S5 = 34; static const int conf_pyranose_1S3 = 35; static const int conf_pyranose_2SO = 36;
            static const int conf_pyranose_5S1 = 37; static const int conf_pyranose_3S1 = 38;

            static const int conf_furanose_3T2 = 39; static const int conf_furanose_3EV = 40; static const int conf_furanose_3T4 = 41; static const int conf_furanose_EV4 = 42;
            static const int conf_furanose_OT4 = 43; static const int conf_furanose_OEV = 44; static const int conf_furanose_OT1 = 45; static const int conf_furanose_EV1 = 46;
            static const int conf_furanose_2T1 = 47; static const int conf_furanose_2EV = 48; static const int conf_furanose_2T3 = 49; static const int conf_furanose_EV3 = 50;
            static const int conf_furanose_4T3 = 51; static const int conf_furanose_4EV = 52; static const int conf_furanose_4TO = 53; static const int conf_furanose_EVO = 54;
            static const int conf_furanose_1TO = 55; static const int conf_furanose_1EV = 56; static const int conf_furanose_1T2 = 57; static const int conf_furanose_EV2 = 58;

            static const int db_not_checked = 9999;  static const int db_not_found = 101010;

            static const int anomer_alpha = 100;     static const int anomer_beta = 101;          static const int handedness_d = 200;     static const int handedness_l = 201;
            static const int type_aldose = 1000;     static const int type_ketose = 2000;


            // We'll use the sugar_ prefix throughout the class for private members

            const MiniMol*                  sugar_parent_molecule;
            const MAtomNonBond*             sugar_parent_molecule_nonbond;
            Coord_orth                      sugar_centre;
            clipper::Vec3<clipper::ftype>   sugar_mean_plane;
            std::vector<MAtom>              sugar_ring_elements;
            std::vector<ftype>              sugar_cremer_pople_params;          // (1) puckering amplitude, (2..) angles
            clipper::MAtom                  sugar_anomeric_carbon;
            clipper::MAtom                  sugar_configurational_carbon;
            clipper::MAtom                  sugar_anomeric_substituent;
            clipper::MAtom                  sugar_configurational_substituent;
            bool                            sugar_found_db;                     // true if the sugar's code is present in the reference data structure
            bool                            sugar_sane;                         // true if passed sanity checks
            bool                            sugar_supported;                    // false if the sugar has no connectivity, missing atoms, etc.
            std::vector<clipper::ftype>     sugar_ring_bonds;                   // bond lengths among ring. [0] is O to anomeric carbon
            std::vector<clipper::ftype>     sugar_ring_angles;                  // ring angles, starting with last_carbon-O-anomeric carbon
            std::vector<clipper::ftype>     sugar_ring_torsion;                 // torsion angles, starting with C5-O5-C1-C2
            int                             sugar_index;                        // 9999 if unchecked, 101010 if absent from the database, 0-400 if found
            String                          sugar_denomination;                 // e.g. alpha-aldopyranose
            String                          sugar_anomer;                       // "alpha", "beta" or "undetermined"
            String                          sugar_handedness;                   // "D", "L" or "undetermined"
            String                          sugar_type;                         // i.e. "aldose" or "ketose"
            String                          sugar_name;
            bool                            sugar_diag_ring;
            bool                            sugar_diag_bonds_rmsd;
            bool                            sugar_diag_angles_rmsd;
            bool                            sugar_diag_anomer;
            bool                            sugar_diag_chirality;
            bool                            sugar_diag_conformation;
            bool                            sugar_diag_puckering;
            bool                            sugar_occupancy_check;
            clipper::String                 sugar_diagnostic;               // full diagnostic to be used in Coot and ccp4i2
            clipper::ftype                  sugar_rscc;                     // RSCC to be used in Coot and ccp4i2
            clipper::ftype                  sugar_accum;                     // accum score, <Fo> in Cryo-EM, <mFo> in Xray.
            clipper::ftype                  sugar_ring_bond_rmsd;
            clipper::ftype                  sugar_ring_angle_rmsd;
            clipper::ftype                  sugar_bfactor;
            int                             sugar_conformation;
            clipper::String                 sugar_alternate_confcode;
            std::vector < MSugar >          sugar_linked_to;                    // size: number of carbon atoms - 1
            clipper::String                 sugar_context;                  // n-glycan, o-glycan or ligand

            // private methods

            std::vector<clipper::ftype>                     cremerPople_pyranose ( const clipper::MiniMol& , clipper::MMonomer); // modifies object
            std::vector<clipper::ftype>                     cremerPople_furanose ( const clipper::MiniMol& , clipper::MMonomer); // modifies object
            std::vector<clipper::MAtom&>                    find_bonded ( const clipper::MAtom& ) const; // accesses object
            int                                             conformationPyranose ( const clipper::ftype&, const clipper::ftype& ) const;
            int                                             conformationFuranose ( const clipper::ftype& ) const;

            std::vector<clipper::MAtom>                     ringMembers ( ) const; // internal function, accesses object
            const std::vector<clipper::MAtom>               findPath ( const clipper::MMonomer&, int, MSugar::visited_arcs& ) const;
            bool                                            closes_ring ( const clipper::MAtom&, MSugar::visited_arcs& ) const;
            const std::vector<clipper::MAtom>               findBonded ( const clipper::MAtom&, MSugar::visited_arcs& ) const; // accesses object
            bool                                            lookup_visited ( const MSugar::visited_arcs&, const std::pair<clipper::MAtom, clipper::MAtom> ) const;
            stereochemistry_pairs                           get_stereochemistry ( const clipper::MiniMol& ); // accesses object
            bool                                            is_stereocentre ( const clipper::MAtom&, const clipper::MiniMol& ); // accesses object
            bool                                            is_part_of_ring ( const clipper::MAtom&, const std::vector<clipper::MAtom> ) const;
            bool                                            bonded ( const clipper::MAtomIndexSymmetry&, const clipper::MAtom& ) const;
            bool                                            bonded ( const clipper::MAtom&, const clipper::MAtom& ) const;
            bool                                            lookup_database ( clipper::String );
            bool                                            examine_ring();

            const char get_altconf ( const clipper::MAtom& ) const;

    }; // class MSugar

    //! A class that holds two MSugar objects
    class MDisaccharide
    {
        public:

            MDisaccharide () {} //!< null constructor
            MDisaccharide ( clipper::MSugar& sugar_one, clipper::MSugar& sugar_two) { this->sugar_one = sugar_one; this->sugar_two = sugar_two; }
            MDisaccharide ( clipper::MiniMol& mmol, const clipper::MAtomNonBond& manb, clipper::MMonomer& mm );
            clipper::MSugar& get_first_sugar () { return sugar_one; }
            clipper::MSugar& get_second_sugar () { return sugar_two; }

            static int search_disaccharides(clipper::String name)
            {
                for (int i = 0; i < clipper::data::disaccharide_database_size ; i++)
                    if (name.trim() == clipper::data::disaccharide_database[i].name_short.trim())
                        return i;

                return -1;
            } //!< returns -1 if not found

        private:
            clipper::MSugar sugar_one;
            clipper::MSugar sugar_two;

    }; // class MDisaccharide

    //! Stores a tree built with Node(s) and Linkage(s), which contain references to MSugar(s)
    class MGlycan
    {
        public:

            MGlycan () { } //!< null constructor
            MGlycan ( clipper::String chain, clipper::MMonomer& root_aa, clipper::MSugar& root_sugar, std::string expression_system = "undefined" );

            class Node;

            class Linkage
            {
                public:

                    /* we need a copy constructor, as we are holding a pointer
                    Linkage ( const Linkage& linkage ) : node  ( linkage.node ),
                                                         index ( linkage.index ),
                                                         type  ( linkage.type ),
                                                         torsion_phi ( linkage.torsion_phi ),
                                                         torsion_psi ( linkage.torsion_psi ) { }
                    */
                    Linkage ( int index, std::string anomericity, int connect_to_id )
                    {
                        node_id = connect_to_id;
                        this->index = index;
                        this->type = anomericity;
                        torsion_phi = torsion_psi = torsion_omega = 0.0;
                    }

                    // Linkage& operator= ( const Linkage& link ) { if ( this != &link ) node = link.node; return *this;  }

                    int get_order () const { return index; }
                    void set_order ( int order ) { index = order; }

                    int get_linked_node_id ( ) const { return node_id; }
                    void modify_linked_node_id ( int modified_connect_to_id ) { node_id = modified_connect_to_id; }

                    std::string get_anomericity ( ) const { return type; }
                    //!< alpha or beta
                    void set_anomericity ( std::string anomericity ) { type = anomericity; }
                    //!< alpha or beta

                    std::string get_description ( bool is_ketose = false )
                    {
                        std::ostringstream message;
                        std::vector< float > torsions = get_torsions();

                        if ( is_ketose ) // ketoses have C1 outside of the ring. C2 is the anomeric centre
                            message << "[ " << get_anomericity() << " 2-" << get_order()
                                    << " ] with phi: " << torsions[0] << "; psi: " << torsions[1];
                        else
                            message << "[ " << get_anomericity() << " 1-" << get_order()
                                    << " ] with phi: " << torsions[0] << "; psi: " << torsions[1];

                        if ( torsions.size() == 3 )
                            message << "; omega: " << torsions[2] << ";";
                        else
                            message << ";";

                        message << annotation;

                        return message.str();

                    }

                    void add_annotation ( std::string message )
                    {
                        annotation = annotation + " " + message;
                    }

                    std::string get_annotation ( ) { return annotation; }

                    std::vector<float> get_torsions () const
                    {
                        std::vector<float> result;
                        result.push_back ( torsion_phi );
                        result.push_back ( torsion_psi );

                        if (index == 6)
                            result.push_back ( torsion_omega );
                        return result;

                    } //!< [0]=phi, [1]=psi, { [2]=omega, if 1-6 linkage }

                    void set_torsions ( float phi, float psi, float omega=0.0 ) { torsion_phi = phi; torsion_psi = psi; torsion_omega=omega; }

                    std::string format() const
                    {
                        std::stringstream s;
                        s << type << " 1-" << index << " to "
                          << node_id;

                        return s.str();
                    }

                private:

                    float torsion_phi;
                    float torsion_psi;
                    float torsion_omega;    // for 1-6 linkages
                    int index;              // carbon to which this is connected
                    int node_id;            // sugar connected to by this linkage
                    std::string type;       // anomer
                    std::string annotation; // include validation information

            }; // class Linkage


            class Node
            {
                public:

                    Node () { initialised = false; }
                    Node ( clipper::MSugar& new_sugar ) { sugar = clipper::MSugar(new_sugar); initialised = true; }
                    Node ( const Node& node )  { this->sugar = clipper::MSugar(node.sugar); this->connections = node.connections; initialised = true; }

                    Node& operator= ( const Node& node ) // this operator seems unnecessary - will keep it in case implementation changes
                    {
                        if ( this != &node )
                        {
                            connections = node.connections;
                            sugar = node.sugar;
                        }
                        initialised = true;
                        return *this;
                    }

                    void add_annotation ( std::string message )
                    {
                        annotation = annotation + " " + message;
                    }

                    std::string get_annotation ( ) { return annotation; }

                    const clipper::MSugar& get_sugar () const { return sugar; }
                    void set_sugar ( clipper::MSugar& new_sugar ) { sugar = clipper::MSugar(new_sugar); }

                    //const std::vector< Linkage >& get_connections () const { return connections; }
                    int add_connection ( const Linkage& connection ) { connections.push_back ( connection ); return connections.size()-1; }

                    const int number_of_connections ( ) const { return connections.size(); }

                    void remove_connection ( const int vectorIndex )
                    {
                        if ( vectorIndex > connections.size() - 1 )
                            connections.pop_back();
                        else
                            connections.erase(connections.begin() + vectorIndex);
                    }


                    Linkage& get_connection ( const int index )
                    {
                        if ( index > connections.size() -1 )
                            return connections.back();
                        else
                            return connections[index];
                    }

                    const std::string format() const
                    {
                        std::stringstream s;

                        s << "Address: " << this
                          << " Sugar: " << sugar.type()
                          << "; Connections: \n";

                        for ( int i = 0; i < number_of_connections() ; i++ )
                            s << connections[i].format() << "\n";

                        return s.str();
                    }

                    const bool is_initialised () { return this->initialised ; }

                private:
                    bool initialised;
                    std::vector < Linkage > connections;
                    clipper::MSugar sugar;
                    std::string annotation;

            }; // class Node

            bool link_sugars  ( int link, clipper::MSugar& first_sugar, clipper::MSugar& next_sugar ); // true if there's been any problem

            const std::pair < clipper::MMonomer, clipper::MSugar >& get_root () const { return this->root; }
            const clipper::String& get_type () const { return kind_of_glycan; } // n-glycan, o-glycan or s-glycan
            std::string get_root_by_name () const { return get_root().first.type().trim() + "-" + get_root().first.id().trim() + "/" + get_chain().substr(0,1); }
            std::string get_root_for_filename () { return "[" + get_chain().trim().substr(0,1) + "]-" + get_root().first.type().trim() + get_root().first.id().trim(); }

            clipper::String print_linear ( const bool print_info, const bool html_format, const bool translate );
            clipper::String print_SVG ( bool vertical, bool print_info, bool colour_gradient );
            
            // NEW FUNCTIONS INTRODUCED DUE TO WURCS IMPLEMENTATION BEGIN //
            char convertNumberToLetter(int number); // need to be relocated, doesn't really belong under ::MGlycan.
            std::vector < std::string > obtain_unique_WURCS_residues();
            const int obtain_total_number_of_glycosidic_bonds();
            clipper::String generate_wurcs ();
            // NEW FUNCTIONS INTRODUCED DUE TO WURCS IMPLEMENTATION END // 


            std::vector < clipper::MSugar >& get_sugars () { return sugars; }

            const Node& get_node ( int index ) const { if (index>node_list.size()-1) return node_list.back(); else return node_list[index]; }

            const clipper::String& get_chain () const { return chain; }
            int number_of_nodes ( ) const { return node_list.size(); }

            void remove_node_at_index ( int index );
            void replace_sugar_at_index ( int index, clipper::MSugar& donor );
            void update_msugar_in_root ( clipper::MSugar& newmsug );

            int get_number_of_connections_at_index ( int index ) const { if (index>node_list.size()-1) return node_list.back().number_of_connections(); else return node_list[index].number_of_connections(); }

            void set_kind_of_glycan ( clipper::String input ) { kind_of_glycan = input; }

            std::vector<float> get_glycosylation_torsions () const
            {
                std::vector<float> result;
                result.push_back ( torsion_phi );
                result.push_back ( torsion_psi );
                return result;
            } //!< [0]=phi, [1]=psi

            void set_glycosylation_torsions ( float phi, float psi ) { torsion_phi = phi; torsion_psi = psi; }

            std::string get_link_description ( )
            {
                std::ostringstream message;

                message << root.second.anomer() << " link. "
                        << "Phi: " << torsion_phi << "; Psi: " << torsion_psi << ";"
                        << link_annotation ;

                return message.str();

            }

            std::string get_root_description ( )
            {
                std::ostringstream message;

                message << get_root_by_name()
                        << root_annotation;

                return message.str();
            }

            void add_root_annotation ( std::string annotation )
            {
                root_annotation = root_annotation + annotation;
            }

            void add_link_annotation ( std::string annotation )
            {
                link_annotation = link_annotation + annotation;
            }

            void set_annotations ( std::string expression_system );  // function that annotates glycobiologic validation

        private:

            clipper::String kind_of_glycan;                 // can be n-glycan, o-glycan or s-glycan
            std::pair < clipper::MMonomer, clipper::MSugar > root;     // this is the root, should be null if this is a ligand saccharide
            std::vector < Node > node_list;                 // interlinked nodes
            clipper::String chain;                          // Chain ID for this glycan
            std::vector < clipper::MSugar > sugars;         // vector of sugars
            clipper::ftype torsion_psi, torsion_phi;        // Torsions of the protein-glycan link
            std::string root_annotation, link_annotation, expression_system;

    }; // class MGlycan


    class MGlycology
    {
        public:

            MGlycology () { } //!< null constructor
            MGlycology ( const clipper::MiniMol&, std::string expression_system = "undefined" );
            MGlycology ( const clipper::MiniMol&, const clipper::MAtomNonBond&, std::string expression_system = "undefined" );

            clipper::MGlycan get_glycan_by_id ( int id );
            clipper::MGlycan get_glycan_by_root ( clipper::MMonomer& root )
            {
                for (int i=0;i<list_of_glycans.size();i++)
                    if (list_of_glycans[i].get_root().first.id()==root.id())
                        return list_of_glycans[i];
                return clipper::MGlycan();
            }
            std::vector < clipper::MGlycan > get_list_of_glycans () const { return list_of_glycans; }
            std::vector < clipper::MSugar > get_sugar_list() { return list_of_sugars; }

        private:

            // private properties
            std::vector < clipper::MSugar >  list_of_sugars;
            std::vector < clipper::MGlycan > list_of_glycans;
            const clipper::MAtomNonBond* manb;
            const clipper::MiniMol * mmol;

            // private methods
            const std::vector < std::pair< clipper::MAtom, clipper::MAtomIndexSymmetry > > get_contacts ( const clipper::MMonomer& mm_one );
            int parse_order ( clipper::String str ) { const char *s = str.c_str(); return atoi(&s[2]); }
            void extend_tree ( clipper::MGlycan& mg, clipper::MSugar& msug );
            const char get_altconf ( const clipper::MAtom& ) const;
            std::string expression_system;

    }; // class MGlycology

} // namespace clipper


#endif
