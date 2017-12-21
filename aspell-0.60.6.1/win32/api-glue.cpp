#include "aspell.h"
#include "language.hpp"
#include "data.hpp"

#ifdef LINUX
#define strcpy_s(dst, sz, src) strncpy(dst, src, sz)
#define sprintf_s snprintf
#endif

/**
 * This is a simple version of aspell's 'create master' function 
 * that can be called without the actual aspell binary.
 *
 * \returns non-zero on success.
 */
int aspell_create_ro_master(void *cfg, const char *InFile, char *ErrOut, int ErrLen)
{
	Config *config = (Config*)cfg;
	if (!config || !InFile)
	{
		strcpy_s(ErrOut, ErrLen, "Error: Param error.");
		return 0;
	}

	char *leaf = strrchr((char*)InFile, OsDirChar);
	if (!leaf)
	{
		strcpy_s(ErrOut, ErrLen, "Error: Failed to find InFile leaf.");
		return 0;
	}
	char *ext = strrchr(++leaf, '.');
	if (!ext)
	{
		strcpy_s(ErrOut, ErrLen, "Error: InFile has no extension.");
		return 0;
	}

	FStream f;
	PosibErr<void> err = f.open(InFile, "r");
	if (err.has_err())
		return 0;

	char rws[256];
	sprintf_s(rws, sizeof(rws), "%.*s.rws", ext - leaf, leaf);

	config->replace("master", rws);

	aspeller::find_language(*config);
	
	IstreamEnumeration *ie = new IstreamEnumeration(f);
	err = aspeller::create_default_readonly_dict(ie, *config);
	if (err.has_err() && ErrOut != NULL && ErrLen > 0)
	{
		const char *e = err.get_err()->mesg;
		if (e && ErrOut)
			strcpy_s(ErrOut, ErrLen, e);
	}

	config->remove("master");
	
	return !err.has_err();
}

extern "C" void prezip_compress(FILE *in_file, FILE *out_file, char *ErrOut, int ErrLen);
extern "C" int prezip_decompress(FILE *in_file, FILE *out_file, char *ErrOut, int ErrLen);

int aspell_prezip(const char *InFile, const char *OutFile, bool Decomp, char *ErrOut, int ErrLen)
{
	if (!InFile || !OutFile)
		return 0;

	FStream in, out;
	if (in.open(InFile, "rb").has_err())
		return 0;
	if (out.open(OutFile, "wb").has_err())
		return 0;

	if (Decomp)
		return prezip_decompress(in.c_stream(), out.c_stream(), ErrOut, ErrLen) == 0;
	else
		prezip_compress(in.c_stream(), out.c_stream(), ErrOut, ErrLen);
	
	return 1;
}
