#include <iostream>
#include <openssl/evp.h>

using std::string;

string md5(const string& content)
{
  EVP_MD_CTX*   context = EVP_MD_CTX_new();
  const EVP_MD* md = EVP_md5();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int  md_len;
  string        output;

  EVP_DigestInit_ex2(context, md, NULL);
  EVP_DigestUpdate(context, content.c_str(), content.length());
  EVP_DigestFinal_ex(context, md_value, &md_len);
  EVP_MD_CTX_free(context);

  output.resize(md_len * 2);
  for (unsigned int i = 0 ; i < md_len ; ++i) {
    sprintf(&output[i * 2], "%02x", md_value[i]);
  }
  return output;
}

#if defined(__UNITTEST__)
int main() {
  std::cout << md5("Terminal Root") << std::endl;
  return 0;
}
#endif
