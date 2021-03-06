#include "connection.hpp"

#define MAXRCVLEN 2000

struct freeMemoryList fml;
using namespace std;

int inspector(int PORTNUM) {

  /**
    Função que raliza inspeção da requisição vinda do browser e da resposta do servidor
    @param PORTNUM: Número da porta que será aberta para receber requisições vindas do browser.
    @return int: Um inteiro indicando sucesso das requisições feitas. Ele pode ser ignorado.
    */

  struct sockaddr_in *dest; /* socket info about the machine connecting to us */
  char rmsg[MAXRCVLEN];
  socklen_t socksize = sizeof(struct sockaddr_in);
  FILE* fd;
  cout << "Tentando criar socket..." << endl;
  int ourSocket = createNewSocket(PORTNUM, 2);

  if ((dest = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))) == NULL) {
    cout << "Erro de alocacao. Abortando\n";
    freeMemory();
    exit(0);
  }
  fml.dest = dest;
  cout << "Aguardando...";
  int consocket = accept(ourSocket, (struct sockaddr *)dest, &socksize);
  int len, i = 10;

  while(--i)
  {
    if((len = read(consocket, rmsg, MAXRCVLEN)) <= 0){
      cout << "Connection close by remote host or some error ocurred. Accepting new connections." << endl;
      close(consocket);
      consocket = accept(ourSocket, (struct sockaddr *)dest, &socksize);
      continue;
    }
    rmsg[len] = '\0';
    if(strstr(rmsg, "POST") != NULL){
      cout << "I am not accepting POST messages. Plese try again." << endl;
      close(consocket);
      consocket = accept(ourSocket, (struct sockaddr *)dest, &socksize);
      continue;
    }
    // printf("%s\n", rmsg);
    fd = fopen("request","w");
    fprintf(fd, "%s", rmsg);
    fclose(fd);
    system("nano request");
    // getchar();

    string request = readTextFile("request");
    vector <unsigned char> response = makeRequest(request);

    writeFile("response", response);
    //Expecting response.txt to contains response from server
    system("nano response");
    cout << "Writing back:" << endl;
    response = readBinaryFile("response");
    unsigned char buff[response.size()];
    for(size_t i = 0; i < response.size(); ++i){
      buff[i] = response[i];
    }
    write(consocket, buff, response.size());
    cout << "Done\n";
  }
  close(consocket);
  close(ourSocket);

  freeMemory();
  dest = NULL;

  return EXIT_SUCCESS;
}


std::vector<unsigned char> readBinaryFile(string filename)
{
  /**
    Função que faz a leitura do binária do arquivo passada como parâmetro
    @param filename: Uma string que apontando o arquivo à ser lido
    @return std::vector<unsigned char>: Um vetor de unsigned char com o bytes lidos do arquivo.
    */

    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<unsigned char> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<unsigned char>(file),
               std::istream_iterator<unsigned char>());

    return vec;
}

std::string readTextFile(string path)
{ 
  /**
    Função que faz a leitura em modo de texto do arquivo passada como parâmetro.
    @param path: Uma string que apontando o arquivo à ser lido.
    @return std::string: Um string contendo o conteúdo do arquivo.
    */
    string result;
    ifstream ifs(path, ios::binary);
    string str(istreambuf_iterator<char>{ifs}, {});

    return str;
}

bool writeFile(string path, vector<unsigned char> dados){
  /**
    Função que faz a escrita dos dados passados como parâmetro em path.
    @param path: Uma string que apontando o arquivo à ser escrito.
    @param dados: Um vector de unsigned char com os dados à serem escritos no arquivo.
    @return bool: Um booleano indicando sucesso na escrito arquivo.
    */
    ofstream fout(path, ios::out | ios::binary);
    fout.write((char*)&dados[0], dados.size() * sizeof(unsigned char));
    fout.close();

    return true;
}