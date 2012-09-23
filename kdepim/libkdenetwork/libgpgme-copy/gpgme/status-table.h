/* Generated automatically by mkstatus */
/* Do not edit! */

struct status_table_s {
    const char *name;
    gpgme_status_code_t code;
};

static struct status_table_s status_table[] = 
{
  { "ABORT", GPGME_STATUS_ABORT },
  { "ALREADY_SIGNED", GPGME_STATUS_ALREADY_SIGNED },
  { "BACKUP_KEY_CREATED", GPGME_STATUS_BACKUP_KEY_CREATED },
  { "BADARMOR", GPGME_STATUS_BADARMOR },
  { "BADMDC", GPGME_STATUS_BADMDC },
  { "BADSIG", GPGME_STATUS_BADSIG },
  { "BAD_PASSPHRASE", GPGME_STATUS_BAD_PASSPHRASE },
  { "BEGIN_DECRYPTION", GPGME_STATUS_BEGIN_DECRYPTION },
  { "BEGIN_ENCRYPTION", GPGME_STATUS_BEGIN_ENCRYPTION },
  { "BEGIN_STREAM", GPGME_STATUS_BEGIN_STREAM },
  { "CARDCTRL", GPGME_STATUS_CARDCTRL },
  { "DECRYPTION_FAILED", GPGME_STATUS_DECRYPTION_FAILED },
  { "DECRYPTION_OKAY", GPGME_STATUS_DECRYPTION_OKAY },
  { "DELETE_PROBLEM", GPGME_STATUS_DELETE_PROBLEM },
  { "ENC_TO", GPGME_STATUS_ENC_TO },
  { "END_DECRYPTION", GPGME_STATUS_END_DECRYPTION },
  { "END_ENCRYPTION", GPGME_STATUS_END_ENCRYPTION },
  { "END_STREAM", GPGME_STATUS_END_STREAM },
  { "ENTER", GPGME_STATUS_ENTER },
  { "ERRMDC", GPGME_STATUS_ERRMDC },
  { "ERROR", GPGME_STATUS_ERROR },
  { "ERRSIG", GPGME_STATUS_ERRSIG },
  { "EXPKEYSIG", GPGME_STATUS_EXPKEYSIG },
  { "EXPSIG", GPGME_STATUS_EXPSIG },
  { "FILE_DONE", GPGME_STATUS_FILE_DONE },
  { "FILE_ERROR", GPGME_STATUS_FILE_ERROR },
  { "FILE_START", GPGME_STATUS_FILE_START },
  { "GET_BOOL", GPGME_STATUS_GET_BOOL },
  { "GET_HIDDEN", GPGME_STATUS_GET_HIDDEN },
  { "GET_LINE", GPGME_STATUS_GET_LINE },
  { "GOODMDC", GPGME_STATUS_GOODMDC },
  { "GOODSIG", GPGME_STATUS_GOODSIG },
  { "GOOD_PASSPHRASE", GPGME_STATUS_GOOD_PASSPHRASE },
  { "GOT_IT", GPGME_STATUS_GOT_IT },
  { "IMPORTED", GPGME_STATUS_IMPORTED },
  { "IMPORT_OK", GPGME_STATUS_IMPORT_OK },
  { "IMPORT_PROBLEM", GPGME_STATUS_IMPORT_PROBLEM },
  { "IMPORT_RES", GPGME_STATUS_IMPORT_RES },
  { "INV_RECP", GPGME_STATUS_INV_RECP },
  { "KEYEXPIRED", GPGME_STATUS_KEYEXPIRED },
  { "KEYREVOKED", GPGME_STATUS_KEYREVOKED },
  { "KEY_CREATED", GPGME_STATUS_KEY_CREATED },
  { "LEAVE", GPGME_STATUS_LEAVE },
  { "MISSING_PASSPHRASE", GPGME_STATUS_MISSING_PASSPHRASE },
  { "NEED_PASSPHRASE", GPGME_STATUS_NEED_PASSPHRASE },
  { "NEED_PASSPHRASE_PIN", GPGME_STATUS_NEED_PASSPHRASE_PIN },
  { "NEED_PASSPHRASE_SYM", GPGME_STATUS_NEED_PASSPHRASE_SYM },
  { "NEWSIG", GPGME_STATUS_NEWSIG },
  { "NODATA", GPGME_STATUS_NODATA },
  { "NOTATION_DATA", GPGME_STATUS_NOTATION_DATA },
  { "NOTATION_NAME", GPGME_STATUS_NOTATION_NAME },
  { "NO_PUBKEY", GPGME_STATUS_NO_PUBKEY },
  { "NO_RECP", GPGME_STATUS_NO_RECP },
  { "NO_SECKEY", GPGME_STATUS_NO_SECKEY },
  { "PKA_TRUST_BAD", GPGME_STATUS_PKA_TRUST_BAD },
  { "PKA_TRUST_GOOD", GPGME_STATUS_PKA_TRUST_GOOD },
  { "PLAINTEXT", GPGME_STATUS_PLAINTEXT },
  { "POLICY_URL", GPGME_STATUS_POLICY_URL },
  { "PROGRESS", GPGME_STATUS_PROGRESS },
  { "REVKEYSIG", GPGME_STATUS_REVKEYSIG },
  { "RSA_OR_IDEA", GPGME_STATUS_RSA_OR_IDEA },
  { "SC_OP_FAILURE", GPGME_STATUS_SC_OP_FAILURE },
  { "SC_OP_SUCCESS", GPGME_STATUS_SC_OP_SUCCESS },
  { "SESSION_KEY", GPGME_STATUS_SESSION_KEY },
  { "SHM_GET", GPGME_STATUS_SHM_GET },
  { "SHM_GET_BOOL", GPGME_STATUS_SHM_GET_BOOL },
  { "SHM_GET_HIDDEN", GPGME_STATUS_SHM_GET_HIDDEN },
  { "SHM_INFO", GPGME_STATUS_SHM_INFO },
  { "SIGEXPIRED", GPGME_STATUS_SIGEXPIRED },
  { "SIG_CREATED", GPGME_STATUS_SIG_CREATED },
  { "SIG_ID", GPGME_STATUS_SIG_ID },
  { "SIG_SUBPACKET", GPGME_STATUS_SIG_SUBPACKET },
  { "TRUNCATED", GPGME_STATUS_TRUNCATED },
  { "TRUST_FULLY", GPGME_STATUS_TRUST_FULLY },
  { "TRUST_MARGINAL", GPGME_STATUS_TRUST_MARGINAL },
  { "TRUST_NEVER", GPGME_STATUS_TRUST_NEVER },
  { "TRUST_ULTIMATE", GPGME_STATUS_TRUST_ULTIMATE },
  { "TRUST_UNDEFINED", GPGME_STATUS_TRUST_UNDEFINED },
  { "UNEXPECTED", GPGME_STATUS_UNEXPECTED },
  { "USERID_HINT", GPGME_STATUS_USERID_HINT },
  { "VALIDSIG", GPGME_STATUS_VALIDSIG },
  {NULL, 0}
};
