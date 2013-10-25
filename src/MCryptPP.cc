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

#include "common.h"

#ifdef USE_LIBMCRYPT

#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>

#include <mcrypt.h>

#include "BitString.h"
#include "EncryptionAlgorithm.h"
#include "EncryptionMode.h"
#include "MCryptPP.h"
#include "MHashKeyGen.h"
#include "error.h"

MCryptPP::MCryptPP ()
{
	ModuleOpen = false ;
}

MCryptPP::MCryptPP (EncryptionAlgorithm a, EncryptionMode m)
{
	open (a, m) ;
}

MCryptPP::~MCryptPP ()
{
	if (ModuleOpen) {
		close() ;
	}
}

void MCryptPP::open (EncryptionAlgorithm a, EncryptionMode m)
{
	std::string tmp1 = a.getStringRep(), tmp2 = m.getStringRep() ;
	char algo[tmp1.size() + 1], mode[tmp2.size() + 1] ;
	strcpy (algo, tmp1.c_str()) ;
	strcpy (mode, tmp2.c_str()) ;

	if ((MCryptD = mcrypt_module_open (algo, MCRYPTPP_LIBDIR, mode, MCRYPTPP_LIBDIR)) == MCRYPT_FAILED) {
		throw SteghideError (_("could not open libmcrypt module \"%s\",\"%s\"."), algo, mode) ;
	}
	ModuleOpen = true ;
}

void MCryptPP::close ()
{
	mcrypt_module_close (MCryptD) ;
	ModuleOpen = false ;
}

BitString MCryptPP::encrypt (BitString p, std::string pp)
{
	p.padRandom (8 * mcrypt_enc_get_block_size (MCryptD)) ; // blocksize is 1 for stream algorithms
	std::vector<unsigned char> ciphertext = _encrypt (p.getBytes(), pp) ;
	return BitString (ciphertext) ;
}

BitString MCryptPP::decrypt (BitString c, std::string pp)
{
	myassert (c.getLength() % (8 * mcrypt_enc_get_block_size (MCryptD)) == 0) ;
	std::vector<unsigned char> plaintext = _decrypt (c.getBytes(), pp) ;
	return BitString (plaintext) ;
}

void* MCryptPP::createKey (std::string pp)
{
	unsigned int keysize = mcrypt_enc_get_key_size (MCryptD) ;
	MHashKeyGen keygen (KEYGEN_MCRYPT, MHASH_MD5, keysize) ;
	std::vector<unsigned char> key = keygen.createKey (pp) ;
	unsigned char *retval = (unsigned char *) s_malloc (keysize) ;
	for (unsigned int i = 0 ; i < keysize ; i++) {
		retval[i] = key[i] ;
	}
	return retval ;
}
std::vector<unsigned char> MCryptPP::_encrypt (std::vector<unsigned char> p, std::string pp)
{
	// genereate key and IV (if needed)
	void *key = createKey (pp) ;
	unsigned char *IV = NULL ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		unsigned int ivsize = mcrypt_enc_get_iv_size (MCryptD) ;
		std::vector<unsigned char> rndIV = RndSrc.getBytes (ivsize) ;
		IV = (unsigned char *) s_malloc (ivsize) ;
		for (unsigned int i = 0 ; i < ivsize ; i++) {
			IV[i] = rndIV[i] ;
		}
	}

	// initialize libmcrypt thread
	unsigned int keysize = mcrypt_enc_get_key_size (MCryptD) ;
	int err = -1 ;
	if ((err = mcrypt_generic_init (MCryptD, key, keysize, IV)) < 0) {
		mcrypt_perror (err) ;
		throw SteghideError (_("could not initialize libmcrypt encryption. see above error messages if any.")) ;
	}

	// copy plaintext
	unsigned int plntextlen = p.size() ;
	myassert (plntextlen % mcrypt_enc_get_block_size (MCryptD) == 0) ;
	unsigned char *plntext = (unsigned char *) s_malloc (plntextlen) ;
	for (unsigned int i = 0 ; i < plntextlen ; i++) {
		plntext[i] = p[i] ;
	}
	
	// encrypt plaintext
	if (mcrypt_generic (MCryptD, plntext, plntextlen) != 0) {
		throw SteghideError (_("could not encrypt data.")) ;
	}

	// create the return value
	std::vector<unsigned char> retval ;
	unsigned int i = 0 ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		unsigned int ivsize = mcrypt_enc_get_iv_size (MCryptD) ;
		retval = std::vector<unsigned char> (ivsize + plntextlen) ;
		for ( ; i < ivsize ; i++) {
			retval[i] = IV[i] ;
		}
	}
	else {
		retval = std::vector<unsigned char> (plntextlen) ;
	}
	for (unsigned int j = 0 ; j < plntextlen ; i++, j++) {
		retval[i] = plntext[j] ;
	}

	// clean up
	if (mcrypt_generic_deinit (MCryptD) < 0) {
		throw SteghideError (_("could not finish libmcrypt encryption.")) ;
	}
	free (plntext) ;
	free (key) ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		free (IV) ;
	}

	return retval ;
}

std::vector<unsigned char> MCryptPP::_decrypt (std::vector<unsigned char> c, std::string pp)
{
	// generate key
	void *key = createKey (pp) ;
	unsigned char *IV = NULL ;
	unsigned int cstart = 0 ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		unsigned int ivsize = mcrypt_enc_get_iv_size (MCryptD) ;
		IV = (unsigned char *) s_malloc (ivsize) ;
		for (unsigned int i = 0 ; i < ivsize ; i++) {
			IV[i] = c[i] ;
		}
		cstart = ivsize ;
	}

	// initialize libmcrypt thread
	unsigned int keysize = mcrypt_enc_get_key_size (MCryptD) ;
	int err = -1 ;
	if ((err = mcrypt_generic_init (MCryptD, key, keysize, IV)) < 0) {
		mcrypt_perror (err) ;
		throw SteghideError (_("could not initialize libmcrypt decryption. see above error messages if any.")) ;
	}

	// copy ciphertext
	unsigned long ciphertextlen = c.size() - cstart ;
	myassert (ciphertextlen % mcrypt_enc_get_block_size (MCryptD) == 0) ;
	unsigned char *ciphertext = (unsigned char *) s_malloc (ciphertextlen) ;
	for (unsigned int i = 0 ; i < ciphertextlen ; i++) {
		ciphertext[i] = c[cstart + i] ;
	}

	// decrypt ciphertext
	if (mdecrypt_generic (MCryptD, ciphertext, ciphertextlen) != 0) {
		throw SteghideError (_("could not decrypt data.")) ;
	}

	// create return value
	std::vector<unsigned char> retval (ciphertextlen) ;
	for (unsigned int i = 0 ; i < ciphertextlen ; i++) {
		retval[i] = ciphertext[i] ;
	}
	
	// clean up
	if (mcrypt_generic_deinit (MCryptD) < 0) {
		throw SteghideError (_("could not finish libmcrypt decryption.")) ;
	}
	free (ciphertext) ;
	free (key) ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		free (IV) ;
	}

	return retval ;
}

EncryptionAlgorithm MCryptPP::getAlgorithm () const
{
	myassert (ModuleOpen) ;
	char *name = mcrypt_enc_get_algorithms_name (MCryptD) ;
	return EncryptionAlgorithm (name) ;
}

EncryptionMode MCryptPP::getMode () const
{
	myassert (ModuleOpen) ;
	char *name = mcrypt_enc_get_modes_name (MCryptD) ;
	return EncryptionMode (name) ;
}

unsigned long MCryptPP::getEncryptedSize (EncryptionAlgorithm a, EncryptionMode m, unsigned long plnsize)
{
	unsigned long retval = 0 ;

	if (a.getIntegerRep() == EncryptionAlgorithm::NONE) {
		retval = plnsize ;
	}
	else {
		std::string tmp1 = a.getStringRep(), tmp2 = m.getStringRep() ;
		char algo[tmp1.size() + 1], mode[tmp2.size() + 1] ;
		strcpy (algo, tmp1.c_str()) ;
		strcpy (mode, tmp2.c_str()) ;

		MCRYPT td ;
		if ((td = mcrypt_module_open (algo, MCRYPTPP_LIBDIR, mode, MCRYPTPP_LIBDIR)) == MCRYPT_FAILED) {
			throw SteghideError (_("could not open libmcrypt module \"%s\",\"%s\"."), algo, mode) ;
		}

		if (mcrypt_enc_mode_has_iv (td)) {
			retval += (8 * mcrypt_enc_get_iv_size(td)) ;
		}

		unsigned long blocks = 0 ;
		const unsigned long blocksize = 8 * mcrypt_enc_get_block_size(td) ; // is 1 for stream algorithms
		if (plnsize % blocksize == 0) {
			blocks = plnsize / blocksize ;
		}
		else {
			blocks = (plnsize / blocksize) + 1;
		}
		retval += (blocks * blocksize) ;

		mcrypt_module_close (td) ;
	}

	return retval ;
}

std::vector<std::string> MCryptPP::getListAlgorithms ()
{
	int size = 0 ;
	char **list = mcrypt_list_algorithms (MCRYPTPP_LIBDIR, &size) ;

	/**
	 * There is a bug in libmcrypt, at least in version 2.5.5 that has the
	 * effect that the algorithm list contains every algorithm twice.
	 **/

	std::vector<std::string> retval ;
	for (int i = 0 ; i < size ; i++) {
		if ((i == 0) || (strcmp(list[i], list[i - 1]) != 0)) { // workaround for the bug mentioned above
			retval.push_back (std::string (list[i])) ;
		}
	}
	mcrypt_free_p (list, size) ;

	return retval ;
}

std::vector<std::string> MCryptPP::getListModes ()
{
	int size = 0 ;
	char **list = mcrypt_list_modes (MCRYPTPP_LIBDIR, &size) ;

	std::vector<std::string> retval ;
	for (int i = 0 ; i < size ; i++) {
		retval.push_back (std::string (list[i])) ;
	}
	mcrypt_free_p (list, size) ;

	return retval ;
}

bool MCryptPP::AlgoSupportsMode (EncryptionAlgorithm a, EncryptionMode m)
{
	std::string tmp1 = a.getStringRep(), tmp2 = m.getStringRep() ;
	char algo[tmp1.size() + 1], mode[tmp2.size() + 1] ;
	strcpy (algo, tmp1.c_str()) ;
	strcpy (mode, tmp2.c_str()) ;

	return (mcrypt_module_is_block_algorithm (algo, MCRYPTPP_LIBDIR) ==
			mcrypt_module_is_block_algorithm_mode (mode, MCRYPTPP_LIBDIR)) ;
}

void *MCryptPP::s_malloc (size_t size)
{
	void *retval = NULL ;
	if ((retval = malloc (size)) == NULL) {
		throw SteghideError (_("could not allocate memory.")) ;
	}
	return retval ;
}

#endif // def USE_LIBMCRYPT
