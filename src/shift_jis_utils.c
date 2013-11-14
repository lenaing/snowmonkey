/*
 * Copyright 2011-2013 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
 * 
 * snowmonkey :
 * ------------
 * This software is a libonsen CLI interface.
 * 
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include "shift_jis_utils.h"

int
onsen_are_shift_jis_bytes(unsigned char first, unsigned char second)
{
    /* JIS X 0208:1997,
     * 
     * Two bytes char :
     * - First char : 0x81 to 0x9f and 0xe0 to 0xef
     * - Second char : 0x40 to 0x7e and 0x80 to 0xfc
     * 
     * Micro$oft code page.
     */
    if ((first >= 0x81 && first <= 0x9f) || (first >= 0xe0 && first <= 0xef)) {
        return (((second >= 0x40) && (second <= 0x7e))
                || ((second >= 0x80) && (second <= 0xfc)));
    }

    /* JIS X 0201:1997, Half kana (0xa1 to 0xdf) */
    if ((first >= 0xa1 && first <= 0xdf) || (second >= 0xa1 && second <=0xdf)) {
        return 1;
    }

    return 0;
}

int
onsen_is_shift_jis(const char *maybeShiftJIS)
{
    int count;
    int len;
    int status;
    unsigned char first; 
    unsigned char second; 

    assert(NULL != maybeShiftJIS);

    status = 1;
    count = 0;
    len = strlen(maybeShiftJIS);

    do {
        first = maybeShiftJIS[count];
        second = maybeShiftJIS[count + 1];
        status = onsen_are_shift_jis_bytes(first, second);
        count++;
    } while (!status && (count < len));

    return status;
}

char *
onsen_shift_jis2utf8 (iconv_t pIconv, char *shiftJIS)
{
    size_t len;
    size_t rc;
    size_t UTF8len;
    char *UTF8;
    char *UTF8tmp;

    assert(NULL != pIconv);
    assert(NULL != shiftJIS);

    len = strlen(shiftJIS);
    if (!len) {
        fprintf(stderr, "Iconv: Input string is empty.");
        return '\0';
    }

    /* Assign enough space to put the UTF-8 string. */
    UTF8len = 2*len;
    UTF8tmp = calloc(UTF8len, 1);
    UTF8 = UTF8tmp;

    rc = iconv(pIconv, (char **)&shiftJIS, &len, &UTF8tmp, &UTF8len);
    if (-1 == (int)rc) {
        fprintf(stderr, "Iconv:");
        switch (errno) {
            case EILSEQ:
                fprintf(stderr, "Invalid multibyte sequence encountered.");
                break;
            case EINVAL:
                fprintf(stderr, "Incomplete multibyte sequence encountered.");
                break;
            case E2BIG:
                fprintf(stderr, "No more room in output buffer.");
                break;
            default:
                fprintf(stderr, "Error: %s.", strerror(errno));
        }
        memcpy(UTF8, "?", len);
    }

    return UTF8;
}
