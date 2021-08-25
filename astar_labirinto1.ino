unsigned long tempo; 
#include <AFMotor.h>
#define trigPin A0 //Pino TRIG do sensor no pino analógica A0
#define echoPin A1 //Pino ECHO do sensor no pino analógica A1
#define trigPin_2 A2
#define echoPin_2 A3
#define num_linhas 5
#define num_colunas 6

// Define o motor1 ligado a conexao 1 (M1 motor shield, motor de trás)
AF_DCMotor motor1(1); 
// Define o motor2 ligado a conexao 4 (M4 motor shield, motor de trás)
AF_DCMotor motor2(2); 
// Define o motor3 ligado a conexao 2 (M2 motor shield, motor da frente)
AF_DCMotor motor3(3);
// Define o motor4 ligado a conexao 3 (M3 motor shield, motor da frente)
AF_DCMotor motor4(4);

typedef struct cel{
  byte g;
  byte h;
  byte f;
  byte nome;
  byte indice; //0 - celula que não foi analisada, 1 - celula analisada e 2 - objeto.
  byte pai;
}CELULA;
CELULA matriz[num_linhas][num_colunas];

CELULA *lista_aberta = (CELULA*)malloc((num_linhas*num_colunas)*sizeof(CELULA));
//CELULA lista_aberta[num_linhas*num_colunas];
byte it_aberta=0;
//CELULA *lista_fechada = (CELULA*)malloc((num_linhas*num_colunas)*sizeof(CELULA));
CELULA lista_fechada[num_linhas*num_colunas];
byte it_fechada=0;
byte primeiro=0;
short ultimo=-1;
byte x_atual;
byte y_atual;
byte primeira_passada=1;
char movimento_robo[num_linhas*num_colunas]; //'b' - baixo, 'c' - cima, 'd' - direita, 'e' - esquerda.
byte contador_movimento=0;
byte flag=0;

byte velocidade = 255;

long duracao;
long HR_dist=0;
long duracao_2;
long HR_dist_2=0;

byte x_inicio = 2;
byte y_inicio = 0;
byte x_fim = 2;
byte y_fim = 5;

void Distancia(){
  digitalWrite(trigPin, LOW); //não envia som
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH); //envia som 
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW); //não envia o som e espera o retorno do som enviado
  duracao = pulseIn(echoPin,HIGH); //Captura a duração em tempo do retorno do som.
  HR_dist = duracao/56; //Calcula a distância
  //Serial.println(HR_dist); //Exibe na Serial a distância
}

void Distancia_2(){
  digitalWrite(trigPin_2, LOW); //não envia som
  delayMicroseconds(2);
  digitalWrite(trigPin_2,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_2,LOW); //não envia o som e espera o retorno do som enviado
  duracao_2 = pulseIn(echoPin_2,HIGH); //Captura a duração em tempo do retorno do som.
  HR_dist_2 = duracao_2/56; //Calcula a distância
  //Serial.println(HR_dist_2); //Exibe na Serial a distância
}

void frente(){
  motor1.setSpeed(velocidade);
  motor1.run(FORWARD);
  motor2.setSpeed(velocidade);
  motor2.run(FORWARD);
  motor3.setSpeed(velocidade);
  motor3.run(FORWARD);
  motor4.setSpeed(velocidade);
  motor4.run(FORWARD);
}

void tras(){
  motor1.setSpeed(velocidade);
  motor1.run(BACKWARD);
  motor2.setSpeed(velocidade);
  motor2.run(BACKWARD);
  motor3.setSpeed(velocidade);
  motor3.run(BACKWARD);
  motor4.setSpeed(velocidade);
  motor4.run(BACKWARD);
}

//gira as rodas da esquerda pra frente e as da direita pra trás, rotacionando o robo pra direita
void direita(){
  motor1.setSpeed(velocidade);
  motor1.run(FORWARD);
  motor2.setSpeed(velocidade);
  motor2.run(FORWARD);
  motor3.setSpeed(velocidade);
  motor3.run(BACKWARD);
  motor4.setSpeed(velocidade);
  motor4.run(BACKWARD);
}

void esquerda(){
  motor1.setSpeed(velocidade);
  motor1.run(BACKWARD);
  motor2.setSpeed(velocidade);
  motor2.run(BACKWARD);
  motor3.setSpeed(velocidade);
  motor3.run(FORWARD);
  motor4.setSpeed(velocidade);
  motor4.run(FORWARD);
}

void parar(){
  motor1.setSpeed(velocidade);
  motor1.run(RELEASE);
  motor2.setSpeed(velocidade);
  motor2.run(RELEASE);
  motor3.setSpeed(velocidade);
  motor3.run(RELEASE);
  motor4.setSpeed(velocidade);
  motor4.run(RELEASE);
  delay(5000);  
}

void celula_baixo(){ 
  frente();
  delay(675);
  parar();  
  /*analisa se tem algum objeto*/
  Distancia(); //chama a função para medir a distância 
  if(HR_dist < 10){    
    tras();
    delay(675);
    //fim, pois o algoritmo A* não realiza o replanejamento do caminho.
    tempo = millis();
    Serial.println(tempo);
    delay(25000);
  }
  else{
    frente();
    delay(900);
  }
}

void celula_cima(){
  tras();
  delay(675);
  parar();
  /*analisa se tem algum objeto*/
  Distancia_2(); //chama a função para medir a distância   
  if(HR_dist_2 < 10){ 
    frente();
    delay(675);
    //fim, pois o algoritmo A* não realiza o replanejamento do caminho.
    tempo = millis();
    Serial.println(tempo);
    delay(25000);
  }
  else{
    tras();
    delay(900);
  }
}

void celula_direita(){
  //gira rodas da esquerda
  direita();
  delay(1400);
  //reto
  frente();
  delay(350);
  parar();
  Distancia();
  if(HR_dist < 10){
    tras();
    delay(350);
    //fim, pois o algoritmo A* não realiza o replanejamento do caminho.
    tempo = millis();
    Serial.println(tempo);
    delay(25000);  
  }
  else{
    frente();
    delay(700); //650    
  }
  //depois gira rodas da direita
  esquerda();
  delay(1400); //faltou força para os motores
}

void celula_esquerda(){
  //gira as rodas da direita
  esquerda();
  delay(1200);
  //reto
  frente();
  delay(550);
  parar();
  Distancia();
  if(HR_dist < 10){
    tras();
    delay(350); 
    //fim, pois o algoritmo A* não realiza o replanejamento do caminho.
    tempo = millis();
    Serial.println(tempo);
    delay(25000); 
  }
  else{
    frente();
    delay(600);  
  }
  //depois gira rodas da esquerda
  direita();
  delay(1200);
}

void h(byte linha, byte coluna){
  byte manhattan;

  manhattan = (abs(x_fim - linha) + abs(y_fim - coluna));

  matriz[linha][coluna].h = manhattan;
}

void g(byte linha, byte coluna){
  byte g_x, g_y;
  
  matriz[linha][coluna].g = (matriz[x_atual][y_atual].g + 1); 
}

void f(byte linha, byte coluna){
  matriz[linha][coluna].f = matriz[linha][coluna].h + matriz[linha][coluna].g;
}

void melhor_caminho(byte x, byte y){
  byte i, j;

  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(matriz[x][y].pai==matriz[i][j].nome){ //se o nome da celula for igual ao pai da celula atual
        if((x-1)==i && y==j){
          movimento_robo[contador_movimento++]='b'; //celula de cima, porém o robô irá fazer o caminho contrário(do início ao fim) por isso celula de baixo
          x=i;
          y=j;
          if(x==x_inicio && y==y_inicio){
            mover();
            tempo = millis();
            Serial.println(tempo);
            delay(25000);
          }
          melhor_caminho(x, y);
        }
        else{
          if((x+1)==i && y==j){
            movimento_robo[contador_movimento++]='c';
            x=i;
            y=j;
            if(x==x_inicio && y==y_inicio){              
              mover();
              tempo = millis();
              Serial.println(tempo);
              delay(25000);
            }
            melhor_caminho(x, y);
          }
          else{
            if(x==i && (y-1)==j){
              movimento_robo[contador_movimento++]='d'; //celula da esquerda, porém armazena celula da direita.
              x=i;
              y=j;
              if(x==x_inicio && y==y_inicio){
                mover();
                tempo = millis();
                Serial.println(tempo);
                delay(25000);

              }
              melhor_caminho(x, y);
            }
            else{
              movimento_robo[contador_movimento++]='e';
              x=i;
              y=j;
              if(x==x_inicio && y==y_inicio){
                mover();
                tempo = millis();
                Serial.println(tempo);
                delay(25000);
              }
              melhor_caminho(x, y);    
            }
          }
        }       
      }
    }
  }
}

void mover(){
  short m;
   for(m=(contador_movimento-1);m>=0;m--){
    if(movimento_robo[m] == 'b'){
      //Serial.println("baixo");
      celula_baixo();
      parar();
    }
    else{
      if(movimento_robo[m] == 'c'){
        //Serial.println("cima");
        celula_cima();
        parar();        
      }
      else{
        if(movimento_robo[m] == 'd'){
          //Serial.println("direita");
          celula_direita();
          parar();
        }
        else{
          //Serial.println("esquerda");
          celula_esquerda();
          parar();
        }          
      }
    }
  }  
}

void iniciar_matriz(){
  byte i, j, cont;
  cont=1;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if((i==1 || i==2 || i==3) && (j>=2 && j<=3)){
        matriz[i][j].g=0;
        matriz[i][j].h=0;
        matriz[i][j].f=0;
        matriz[i][j].nome=cont++;
        matriz[i][j].indice=2;
        matriz[i][j].pai=0;
      }
      else{
        matriz[i][j].g=0;
        matriz[i][j].h=0;
        matriz[i][j].f=0;
        matriz[i][j].nome=cont++;
        matriz[i][j].indice=0;
        matriz[i][j].pai=0;
      }
    }
  }
}

void iniciar_lista_aberta(){
  byte i;
  for(i=0;i<num_linhas*num_colunas;i++){
    lista_aberta[i].g=0;
    lista_aberta[i].h=0;
    lista_aberta[i].f=0;
    lista_aberta[i].nome=0;
    lista_aberta[i].indice=0;
    lista_aberta[i].pai=0;
  }
}

void iniciar_lista_fechada(){
  byte i;
  for(i=0;i<num_linhas*num_colunas;i++){
    lista_fechada[i].g=0;
    lista_fechada[i].h=0;
    lista_fechada[i].f=0;
    lista_fechada[i].nome=0;
    lista_fechada[i].indice=0;
    lista_fechada[i].pai=0;
  }
}  

void detectar_vizinhos(byte x_atual, byte y_atual){
  byte i, j, n;
  //nao pode considerar objeto e quem esta na lista aberta e fechada
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(i==(x_atual-1) && j==y_atual && matriz[i][j].indice==0){ //se for um vizinho
        if(i==x_fim && j==y_fim){ //se esse vizinho for a celula objetivo
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          //Serial.println("1");
          melhor_caminho(i, j);
        }else{
          //verifica_custo();
          //antes de verificar o custo e adicionar na lista aberta, deve-se verificar se a celula analisada não está na lista aberta e nem na fechada.
          //H
          h(i, j); //linha corrente e coluna corrente
          //G
          g(i, j); 
          //F
          f(i, j);
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          lista_aberta[it_aberta]=matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].indice=1;        
        }
      }
      if(i==x_atual && j==(y_atual+1) && matriz[i][j].indice==0){
        if(i==x_fim && j==y_fim){ //se esse vizinho for a celula objetivo
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          //Serial.println("2");
          melhor_caminho(i, j);

        }
        else{
          //calcular f, g, h
          h(i, j);
          g(i, j);
          f(i, j);
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          lista_aberta[it_aberta]=matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].indice=1;
        }
      }
      if(i==(x_atual+1) && j==y_atual && matriz[i][j].indice==0){
        if(i==x_fim && j==y_fim){ //se esse vizinho for a celula objetivo
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          //Serial.println("3");
          melhor_caminho(i, j);
          //mover();

        }
        else{
          h(i, j);
          g(i, j);
          f(i, j);
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          lista_aberta[it_aberta]=matriz[i][j];
          it_aberta++; 
          ultimo++; 
          matriz[i][j].indice=1;   
        }  
      }
      if(i==x_atual && j==(y_atual-1) && matriz[i][j].indice==0){
        if(i==x_fim && j==y_fim){ //se esse vizinho for a celula objetivo
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          //Serial.println("4");
          melhor_caminho(i, j);

        }
        else{
          h(i, j);
          g(i, j);
          f(i, j);
          matriz[i][j].pai = matriz[x_atual][y_atual].nome;
          lista_aberta[it_aberta]=matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].indice=1;   
        }
      
      }
    }
  }
  //reordenar a lista aberta
}

void insertion_sort(byte primeiro, short ultimo){
  byte i, j;
  CELULA key;
  for (i = (primeiro+1); i <= ultimo; i++) {
    key = lista_aberta[i];
    j = i - 1;
    while (j >= primeiro && lista_aberta[j].f > key.f) {
        lista_aberta[j + 1] = lista_aberta[j];
        j = j - 1;
    }
    lista_aberta[j + 1] = key;
  }
}

void busca_celula_analisar(){
  byte i, j, contador;
  contador=0;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      contador++;
      if(lista_aberta[primeiro].nome == contador){
        x_atual=i;
        y_atual=j;
      }     
    }
  }
}

void imprime_lista_aberta(byte primerio){
  byte i;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    Serial.print(lista_aberta[i].nome);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void imprime_lista_fechada(){
  byte i;
  for(i=0;i<num_linhas*num_colunas;i++){
    Serial.print(lista_fechada[i].nome);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void imprimir_matriz_nome(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].nome);
      Serial.print(' ');
    }
    Serial.println(" ");
  }
}

void imprimir_matriz_f(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].f);
      Serial.print(' ');
    }
    Serial.println(" ");
  }
}

void imprimir_matriz_indice(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].indice);
      Serial.print(' ');
    }
    Serial.println(" ");
  }  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin_2, OUTPUT);
  pinMode(echoPin_2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:  
  if(primeira_passada==1){
    iniciar_matriz(); //preenche a matriz
    //imprimir_matriz_indice();
    iniciar_lista_aberta();
    iniciar_lista_fechada();
    //adiciona a celula início na lista aberta
    x_atual=x_inicio;
    y_atual=y_inicio;
    
    lista_aberta[it_aberta] = matriz[x_atual][y_atual];
    it_aberta++;
    ultimo++;
    matriz[x_atual][y_atual].indice=1;
  }
  primeira_passada = 0;  
  //verificar quem são os vizinhos e também adicionar na lista aberta
  detectar_vizinhos(x_atual, y_atual);
  lista_fechada[it_fechada]=lista_aberta[primeiro];
  it_fechada++;
  primeiro++; //novo primeiro
  insertion_sort(primeiro, ultimo);
  busca_celula_analisar();

}
