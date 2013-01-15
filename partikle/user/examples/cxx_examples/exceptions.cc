// exceptions
extern "C" int printf (char *str, ...);

int main () {
  char myarray[10];
  try {
    for (int n=0; n<=10; n++)
      {
	if (n>9) throw 1;
	myarray[n]='z';
      }
    printf ("Paso\n");
  }
  catch (int a)
  {
    printf ("There was an exception!!\n");
  }
  
  return 0;
}
