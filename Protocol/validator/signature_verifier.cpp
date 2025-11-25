#include "signature_verifier.h"
#include "../utils/crypto_utils.h"
#include <iostream>

namespace dip::security {

bool SignatureVerifier::verify_manifest_signature(const string &manifest_json, const string &pubkey_pem, const std::string &sig_hex) {
    // canonicalize manifest_json if necessary (sort keys) — omitted here
    bool ok = verify_signature_pem(pubkey_pem, manifest_json, sig_hex);
    if (!ok) std::cerr << "[SignatureVerifier] manifest signature invalid\n";
    return ok;
}

} // namespace dip::security
