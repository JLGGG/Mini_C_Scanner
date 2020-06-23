/*
    Mini C has two types of comments: text comment and line comment.
*/
/**
    문서화 주석의 경우 내용을 출력해야 합니다.
    문서화 주석 부분입니다.
*/

void main()
{ 
  printf("\a \b \f \n \r \t \v \\ \' \" \? \e");
  JUMP:
  int i;
  double d=100.5236;
  
  i = 1000;	// i sets 100
  write(i);	// printf it
  for( ; ;);
  switch(){
    case : break;
  }
  while(){
    d=0.333;
    continue;
  }
  goto JUMP;
}
