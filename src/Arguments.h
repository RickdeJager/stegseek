/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 1999-2003 Stefan Hetzl <shetzl@chello.at>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef SH_ARGUMENTS_H
#define SH_ARGUMENTS_H

#include <string>
#include <vector>

#include "Arg.h"

// to include DEBUG if defined
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * \class Arguments
 * \brief parsing and data representation of command-line arguments
 **/
class Arguments {
	public:
	Arguments (void) {} ;

	/**
	 * initialize this Arguments object with argc and argv
	 **/
	Arguments (int argc, char *argv[]) ;

	/**
	 * parse Argc and Argv filling the Arg* member variable for later access
	 **/
	void parse (void) ;

	/**
	 * is standard input used ? - according to the given arguments
	 **/
	bool stdin_isused (void) const ;

	/// the command to be executed in this session
	ArgCommand		Command ;
	/// the name of the command to be executed in this session (as supplied by the user)
	std::string		CommandString ;

	/// the embed file name, "" if stdin
	ArgString		EmbFn ;
	/// the extract file name, "" if stdout
	ArgString		ExtFn ;
	/// the cover file name, "" if stdin
	ArgString		CvrFn ;
	/// the stego file name, "" if stdout/stdin
	ArgString		StgFn ;
	ArgString		Passphrase ;
	ArgBool			Checksum ;
	ArgInt			Compression ;
	ArgBool			EmbedEmbFn ;
	ArgEncAlgo		EncAlgo ;
	ArgEncMode		EncMode ;
	ArgULong		Radius ;
	ArgFloat		Goal ;
	ArgBool			Force ;
	ArgVerbosity	Verbosity ;
	ArgDebugCommand	DebugCommand ;
	ArgBool			Check ;
	ArgStringList	FileList ;
	ArgUInt			DebugLevel ;
	ArgUInt			GmlGraphRecDepth ;
	ArgUInt			GmlStartVertex ;

	std::string getPassphrase (bool doublecheck = false) ;

	private:
	typedef std::vector<std::string>::const_iterator ArgIt ;

	static const int		NoCompression = 0 ;

	static const EncryptionAlgorithm Default_EncAlgo ;
	static const EncryptionMode Default_EncMode ;
	static const bool		Default_Checksum = true ;
	static const int		Default_Compression = 9 ; // slowest, but smallest
	static const bool		Default_EmbedEmbFn = true ;
	static const bool		Default_Force = false ;
	static const VERBOSITY	Default_Verbosity = NORMAL ;
	static const unsigned long	Default_Radius = 0 ; // there is no default radius for all file formats
	static const unsigned int	Max_Algorithm = 3 ;
	static const float		Default_Goal = 100.0 ;
	static const DEBUGCOMMAND	Default_DebugCommand = NONE ;
	static const bool		Default_Check = false ;
	static const unsigned int	Default_DebugLevel = 0 ;
	static const unsigned int	Default_GmlGraphRecDepth = 0 ;
	static const unsigned int	Default_GmlStartVertex = 0 ;

	/**
	 * parse the command
	 *
	 * Note: parse_Command is the only parse_* function that requires curarg to be a command.
	 * (because the command is the only argument with a fixed position).
	 **/
	void parse_Command (ArgIt& curarg) ;

	/**
	 * test if curarg points to an emb filename argument and if yes: parse it
	 * \return true iff one or more arguments have been parsed
	 **/
	bool parse_EmbFn (ArgIt& curarg) ;

	bool parse_ExtFn (ArgIt& curarg) ;
	bool parse_CvrFn (ArgIt& curarg) ;
	bool parse_StgFn (ArgIt& curarg) ;
	bool parse_Passphrase (ArgIt& curarg) ;
	bool parse_Checksum (ArgIt& curarg) ;
	bool parse_Compression (ArgIt& curarg) ;
	bool parse_EmbedEmbFn (ArgIt& curarg) ;
	bool parse_Encryption (ArgIt& curarg) ;
	bool parse_Radius (ArgIt& curarg) ;
	bool parse_Goal (ArgIt& curarg) ;
	bool parse_Force (ArgIt& curarg) ;
	bool parse_Verbosity (ArgIt& curarg) ;
	bool parse_Debug (ArgIt& curarg) ;

	void setDefaults (void) ;

	std::vector<std::string> TheArguments ;
} ;

// gcc does not support the export keyword
#include "Arg.cc"

#endif /* ndef SH_ARGUMENTS_H */
