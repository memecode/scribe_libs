#include "language.hpp"
#include "data.hpp"

/**
 * This is a simple version of aspell's 'create master' function 
 * that can be called without the actual aspell binary.
 *
 * \returns non-zero on success.
 */
int aspell_create_ro_master(const char *InFile)
{
	FStream f;
	PosibErr<void> err = f.open(InFile, "r");
	if (err.has_err())
		return 0;

	StackPtr<Config> config;
	config = new_config();
	aspeller::find_language(*config);
	
	return !aspeller::create_default_readonly_dict(new IstreamEnumeration(f), *config).has_err();
}
