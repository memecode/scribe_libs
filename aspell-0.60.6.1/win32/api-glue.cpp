#include "language.hpp"
#include "data.hpp"

/**
 * This is a simple version of aspell's 'create master' function 
 * that can be called without the actual aspell binary.
 *
 * \returns non-zero on success.
 */
int aspell_create_ro_master(Config *config, const char *InFile)
{
	if (!config)
		return 0;

	FStream f;
	PosibErr<void> err = f.open(InFile, "r");
	if (err.has_err())
		return 0;

	aspeller::find_language(*config);
	return !aspeller::create_default_readonly_dict(new IstreamEnumeration(f), *config).has_err();
}

extern "C" void prezip_compress(FILE *in_file, FILE *out_file);
extern "C" int prezip_decompress(FILE *in_file, FILE *out_file);

int aspell_prezip(const char *InFile, const char *OutFile, bool Decomp)
{
	if (!InFile || !OutFile)
		return 0;

	FStream in, out;
	if (in.open(InFile, "rb").has_err())
		return 0;
	if (out.open(OutFile, "wb").has_err())
		return 0;

	if (Decomp)
		return prezip_decompress(in.c_stream(), out.c_stream()) == 0;
	else
		prezip_compress(in.c_stream(), out.c_stream());
	
	return 1;
}
