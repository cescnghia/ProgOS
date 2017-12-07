// C99
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAX_IMAGE_HEIGHT 100
#define MAX_IMAGE_WIDTH 100 
#define MAX_FILE_NAME 1024
#define MASQUE_SIZE 3

typedef double Pixel;

typedef struct {
	unsigned int mHeight;
	unsigned int mWidth;
	Pixel tab[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH] ;
} Image;

// Prototypes

Image diamond(unsigned int, unsigned int, unsigned int);
Image demandeImage();
unsigned int demandeDiagonale(unsigned int);
void display(FILE*,Image);
int write_to_file(char[],Image);
void demandeFileName(char[]);
Image read_from_file(char[]);
Image filter(Image,Image);
int validHeight(unsigned);
int validWidth(unsigned);
int min(int, int);

// main function
int main(void){
	Image img = demandeImage();
	unsigned int diagonal = demandeDiagonale(min(img.mHeight,img.mWidth));
	img = diamond(img.mWidth,img.mHeight,diagonal);
	printf("Voici votre image: \n");
	display(stdout,img);

	char pathToFile[MAX_FILE_NAME];
	// Une path valide: /Users/xxxxxx/Desktop/test.txt 
	// c-à-d je vais écrire dans le fichier test.txt qui se situe au Desktop
	demandeFileName(pathToFile);
	write_to_file(pathToFile,img);
	Image readImage = read_from_file(pathToFile);
	printf("Voici l'image que j'ai lu : \n");
	display(stdout,readImage);

	Image masque = {MASQUE_SIZE,MASQUE_SIZE,{{-2.0,-2.0,-2.0}, { 0.0, 0.0, 0.0}, { 2.0, 2.0, 2.0}}};
	printf("On va maintenent FILTRER votre image: \n");
	Image filterImage = filter(readImage,masque);
	display(stdout,filterImage);
	return 0;
}
// fonction qui prend 3 valeurs: hauteur, largeur, et le diagonale et puis désiner et retourner une image... 
Image diamond(unsigned width, unsigned height, unsigned int D){
	Image result;
	if ( (!validHeight(height)) || (!validWidth(width)) || (D < 1 || D > min(width,height)) ){
		fprintf(stderr, "Comment je peux créer une image avec des arguments bizares ?\n");
	} else {
		int centreX = width/2;
		int centreY = height/2;
		int i, j, x1, x2;
		Pixel pixel;
		for (i=0; i<=height/2; i++){
			x1 = centreX - (i - centreY + D/2);
			x2 = centreX + (i - centreY + D/2);
			for (j=0; j<width; j++){
				if (j<x1 || j>x2){
					pixel = 0.0;
				} else {
					pixel = 1.0;
				}
				result.tab[i][j] = pixel ;
				result.tab[height-1-i][j] = pixel ; // symétrique
			}
		}
		result.mHeight = height;
		result.mWidth = width; 
	}
	return result;
}


// fonction qui demande d'utilisateur donner la taille de hauteur, largeur et retourne une creux image avec son hauteur, largeur.
Image demandeImage(){
	Image result;
	unsigned height, width;
	int err;
	printf("Veuillez donner les valeurs de hauteur et largeur pour l'image\n");
	do{
		printf("Un nombre plus grand que 0 et plus petit égal que %d pour le hauteur :", MAX_IMAGE_HEIGHT); 
		fflush(stdout);
		err = scanf("%d", &height);
		if ( (err != 1) || (!validHeight(height)) ){
			printf("Hauteur doit etre un NOMBRE plus grand que 0 et plus petit égal que %d svp !!!\n",MAX_IMAGE_HEIGHT);
			while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
		}
	} while ( (err != 1) || (!validHeight(height)) );

	do{
		printf("Un nombre plus grand que 0 et plus petit égal que %d pour le largeur :", MAX_IMAGE_WIDTH); 
		fflush(stdout);
		err = scanf("%d", &width);
		if ( (err != 1)  || (!validWidth(width)) ){
			printf("Largeur doit etre un NOMBRE plus grand que 0 et plus petit égal que %d svp !!!\n",MAX_IMAGE_WIDTH);
			while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
		}
	} while ( (err != 1) || (!validWidth(width)) );

	result.mHeight = height|1;
	result.mWidth = width|1;
	return result;
}

// fonction qui demande d'utilisateur donner la taille de la diagonale
unsigned int demandeDiagonale(unsigned int bound){
	unsigned int diagonale;
	int err;
	do{
		printf("Donnez la longueur de la diagonale (plus grand que 0 et plus petit ou égal que %d) :",bound); 
		fflush(stdout);
		err = scanf("%d", &diagonale);
		if ( (err != 1) || (diagonale>bound) || diagonale < 1){
			printf("Je vous demande un NOMBRE plus grand que 0 et plus petit ou égal %d svp !!!\n",bound);
			/* Vider le tampon d'entree */
			while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
		}
	} while (diagonale > bound || diagonale < 1);

	return diagonale ;
}

// une fonction qui prend une image et a flot, elle va afficher dans le flot
void display(FILE* file,Image img){
	if (file == NULL || !validHeight(img.mHeight) || !validWidth(img.mWidth) ){
		fprintf(stderr, "Comment je peux désiner une image avec des arguments bizarres ?\n");
	} else {
		int i,j;
		for(i=0; i<img.mHeight; i++){
			for(j=0; j<img.mWidth; j++){
				if(img.tab[i][j] == 0.0){
					fprintf(file,". ");
				} else if (img.tab[i][j] == 1.0) {
					fprintf(file,"+ ");
				} else {
					fprintf(file, "* ");
				}
			}
			fprintf(file,"\n");
		}
	}
}

// une fonction qui prend une "absolute path" vers un fichier et une image pour écrire dans le fichier
int write_to_file(char pathToFile[], Image img){
	FILE* file = NULL;
	int err;
	unsigned int height = img.mHeight;
	unsigned int width = img.mWidth;
	if (pathToFile[0] != '\0'){
		file = fopen(pathToFile,"w");
		if (file == NULL){
			fprintf(stderr,"Erreur, on ne peut pas ouvrir le fichier %s\n",pathToFile);
			fprintf(stderr, "%s\n",strerror(errno));
			err = -1;
		} else {
			fprintf(file,"%d\n",width);
			fprintf(file,"%d\n",height);
			display(file,img);
			err =  0;
		}
		fclose(file);
	}
	return err;
}
// une fonction qui prend un tableau de char et puis demander le "absolute path" et le stocker dans le tableau
void demandeFileName(char pathToFile[]){
	/* Vider sdtin */
	fseek(stdin,0,SEEK_END);
	int lenght;
	do {
		printf("Donnez la absolute path (longueur doit etre plus petit que %d) vers le fichier svp: ",MAX_FILE_NAME);
		fgets(pathToFile,MAX_FILE_NAME+1,stdin);
		lenght = strlen(pathToFile) - 1 ;
		if ((lenght >= 0) && (pathToFile[lenght] == '\n')){
			pathToFile[lenght] = '\0' ;
		}
	} while ((lenght < 1) && !feof(stdin) && !ferror(stdin));
}

// une fonction qui prend un tableau de char qui contient un "absolute path", on va lire l'image dans le fichier et la retourner
Image read_from_file(char pathToFile[]){
	FILE* file = NULL ;
	Image result;
	unsigned int width = 0;
	unsigned int height = 0;
	// parce que entre les 2 charactères, on a aussi une espace
	char readLine[2*width];
	file = fopen(pathToFile,"r");
	if (file == NULL){
		fprintf(stderr,"Erreur, on ne peut pas ouvrir le fichier %s\n",pathToFile);
		fprintf(stderr, "%s\n",strerror(errno));
	} else {
		fseek(stdin,0,SEEK_END);
		if ( (2 == fscanf(file,"%d %d",&width,&height)) && validWidth(width) && validHeight(height) ){
			int i,j; 
			char c ;
			// utiliser fgets pour sauter ligne par ligne
			while ( (i<height) && (fgets(readLine,2*width,file) != NULL) )  {
				for(j = 0; j < width*2; j++){
					c = getc(file);
					if(c == '+'){
						result.tab[i][j/2] = 1.0;
					} else if (c == '.') {
						result.tab[i][j/2] = 0.0;
					}
				}
				i++ ; // nouvelle ligne
			}		
		}
	}
	fclose(file);
	result.mWidth = width ;
	result.mHeight = height;
	return result;
}

//une fonction qui prend une image et une masque et puis filter l'image et la retourner
Image filter(Image img, Image masque){
	Image result ;

	if (validHeight(img.mHeight) && validWidth(img.mWidth) && validHeight(masque.mHeight) && validWidth(masque.mWidth) ){
		int height = img.mHeight;
		int width = img.mWidth;
		int i,j,k,l;
		int indexX, indexY;
		Pixel temp = 0; 
		// encore symétrique parce l'image est symétrique
		for(i = 0; i <= height/2; i++){
			for(j = 0; j < width; j++){
				if(i != height/2){
					for(k = 0; k < MASQUE_SIZE; k++){
						for(l = 0; l < MASQUE_SIZE; l++){
							indexX = ((i + MASQUE_SIZE/2 - k) + height) % height;
							indexY = ((j + MASQUE_SIZE/2 - l) + width) % width;
							temp += img.tab[indexX][indexY]*masque.tab[k][l];
						}
					}
				}
				result.tab[i][j] = temp;
				result.tab[height-1-i][j] = temp;
				temp = 0 ; // reset 
			}
		}
		result.mHeight = height;
		result.mWidth = width;
	} else {
		result.mHeight = 0;
		result.mWidth = 0;
	}
	return result ;
}

// une fonction qui retourner la minimale entre deux nombres
int min(int a, int b){
	if (a <= b){
		return a;
	} else {
		return b;
	}
}

//une fonction pour checker si le hauteur est validé ?
int validHeight(unsigned height){
	if (height > 0 && height < MAX_IMAGE_HEIGHT){
		return 1;
	} else {
		return 0;
	}
}

//une fonction pour checker si le largeur est validé ?
int validWidth(unsigned width){
	if (width > 0 && width < MAX_IMAGE_WIDTH){
		return 1;
	} else {
		return 0;
	}
}