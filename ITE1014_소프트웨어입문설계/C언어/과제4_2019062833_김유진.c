#include <stdio.h>
#include <stdlib.h>
//���� ���ڿ��迭�� ũ�⸦ �������ָ� �������ش�
char fir[30], sec[30],menu[5];

//���ϱ��Լ�
float add(float a, float b) {
	float result;
	result = a + b;
	return result;

}
//�����Լ�
float sub(float a, float b) {
	float result;
	result = a - b;
	return result;

}
//���ϱ� �Լ�
float mul(float a, float b) {
	float result;
	result = a * b;
	return result;
}
//�������Լ�
float di(float a, float b) {
	float result;
	result = a / b;
	return result;
}
//�����Լ� ����
int main() {
	
	//����� ������ �Ǽ����� ����
	int menu2=10,fir2,sec2,count=0;
	float re,result;
	
	//ó�� ���� ȭ�� ���
	printf("------------------------\n    Last calculator\n------------------------\n");
	
	//�޴��� ��� �Է� �޾ƾ��ϹǷ� while�� ���
	while (1) {

		//������ ��� ���Ҵ� �� ī��Ʈ
		count++;
		
		
		
		//�޴����
		printf("1. add\n2. sub\n3. mul\n4. div\n0 exit\n");
		printf("------------------------\n");
		//����ڿ��� �޴� �Է¹ޱ�
		while (1) {
			scanf(" %c", menu);
			menu2 = atoi(menu);
			//���ڸ� �Է� �޾��� ��� �ٽ� �ް���
			if (menu2 == 0 && menu[0]!='0') {
					
				printf("wrong input!! input again\n");
				continue;
			}
			//�޴����������� ���� �Է��� �� �����޼����� ����
			else if(menu2!=1 && menu2!=2 && menu2!=3 && menu2!=4 && menu2!=0){
				

				printf("Wrong command!!!!!\n");
				continue;
			}
			//���ڸ� ���Է¹޾��� ��� while�� Ż��
			else {
				break;
			}
		}
		
		
	
		//���ϱ��Լ��� �Է¹޾�����
		if (menu2 == 1) {

			//ó�������϶�
			if (count == 1) {
				//ù��° ���� �Է¹ޱ�
				printf("input first operand : ");
				//���� �Է¹������� �����̸� �ٽ� �Է¹޴´�
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}
				
				//�ι�° ���� �Է¹ޱ�
				printf("input second operand : ");
				//�����̸� �ٽ� �Է� �޴´�
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}
				//���ϱ� �Լ��� ȣ���ϰ� �� ���� ����Ʈ�Ѵ�
				printf("result is %0.4f\n", add(fir2, sec2));
				//���� ���꿡 �Ѱ��� ������� ������ �־��ش�
				result = add(fir2, sec2);
			}
			//�ι�° �̻��� ������ �� 
			else {
				//���� ���ට �޾Ƴ��� ������� ����Ʈ�Ѵ�
				printf("%0.4f + ", result);
				//�޾Ƴ��� ��� ���� �߰��� �������� ���� �Է¹޴´�
				//���ڸ� �Է¹޾��� ��� �ٽ� �Է¹޴´�
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again\n%0.4f +  ",result);
						continue;
					}
					else {
						break;
					}
				}
				//������� ���ι��� ���� �����ؼ� �ٽ� ���� �������� �Ѱ��� ��������� �־��ش�
				printf("result is %0.4f\n", add(result, sec2));
				re = add(result, sec2);
				result = re;
			}
		}
		//�����Լ�����
		else if (menu2 == 2) {

			//ó�������϶�
			if (count == 1) {
				printf("input first operand : ");
				//ó������ �Է¹޵� ���ڸ� �Է¹��� �� �ٽ� �Է¹޴´�
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//�ι�° ���� �Է¹޵� ���ڸ� �Է¹��� �� �ٽ� �Է¹޴´�
				printf("input second operand : ");
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//������� ����Ʈ�ϰ� ���� ���꿡 �Ѱ��� ������ �����Ѵ�
				printf("result is %0.4f\n", sub(fir2, sec2));
				result = sub(fir2, sec2);
			}
			//�ι�° �̻��� �����϶�
			else {
				//�ռ����࿡�� �޾Ƴ��� ������� ����Ʈ�Ѵ�
				printf("%0.4f - ", result);
				//������ ���ο� ���� �Է¹޵� ���ڸ� �Է� ���� �� �ٽ� �Է� �޴´�
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again\n%0.4f - ",result);
						continue;
					}
					else {
						break;
					}
				}
				
				//�Ѱܹ��� ������� ���ι��� ���� �����Ͽ� �� ����� ����Ʈ�ϰ� ������������ �Ѱ��� ������� ������ ����
				printf("result is %0.4f\n", sub(result, sec2));
				re = sub(result, sec2);
				result = re;
			}

		}
		//���ϱ⿬�����
		else if (menu2 == 3) {

			//ó�������϶�
			if (count == 1) {
				//ù��° ���� �Է¹޵� ���ڸ� �Է¹����� ���Է¹޴´�
				printf("input first operand : ");
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//�ι�°���� �Է¹޵� ���ڸ� �Է¹��� �� ���Է� �޴´�
				printf("input second operand : ");
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}
				//������� ����Ʈ�ϰ� ���� ���꿡 �Ѱ��� ������� �����Ѵ�
				printf("result is %0.4f\n", mul(fir2, sec2));
				result = mul(fir2, sec2);
			}
			//�ι�° �̻��� �����϶�
			else {
				//�Ѱܹ��� ����� ����Ʈ
				printf("%0.4f * ", result);
				//���ο� ���� �Է¹޵� ���ڸ� �Է¹��� �� ���Է¹���
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again\n%0.4f * ",result);
						continue;
					}
					else {
						break;
					}
				}
				
				//�Ѱܹ��� ������� ���ι������� �����ѵ� �� ������� ����Ʈ�ϰ� �������꿡 �Ѱ��� ��������� �����Ѵ�
				printf("result is %0.4f\n", mul(result, sec2));
				re = mul(result, sec2);
				result = re;
			}
		}
		//�����⿬�����
		else if (menu2 == 4) {

			//ó�������϶�
			if (count == 1) {
				//ù��°���� �Է¹޵� ���ڸ� ������ ���Է¹ޱ�
				printf("input first operand : ");
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//�ι�° ���� �Է¹޵� ���ڸ� �ްų� 0�̸� �ٽ� �Է¹ޱ�
				printf("input second operand : ");
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else if (sec[0] == '0') {
						printf("can't divide by zero. input again : ");
						continue;
					}
					else {
						break;
					}
				}
				//�μ��� ������ �� �� ������� ����Ʈ�ϰ� �������꿡 �Ѱ��ֱ� ���� ��������� �����Ѵ�
				printf("result is %0.4f\n", di(fir2, sec2));
				result = di(fir2, sec2);

			}
			//�ι�°�̻��� �����϶�
			else {

				//���ǿ��꿡�� �޾ƿ� ������� ����Ʈ�ϰ� ���ο���� �Է¹޵� �����̰ų� 0 �̸� ���Է¹޴´�
				printf("%0.4f / ", result);
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec[0] == '0') {
						printf("can't divide by zero. input again\n%0.4f / ",result);
						continue;
					}
					else if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					break;
				}
				
				//���ι������� �տ��� �޾ƿ� ������� �����ѵ� ����Ʈ�ϰ� �ٽ� ��������� �����ؼ� ���� �������� �Ѱ��ش�
				printf("result is %0.4f\n", di(result, sec2));
				re = di(result, sec2);
				result = re;
			}

		}
		//0�� �Է¹��� �� ���α׷��� �����Ų��
		else if (menu2 == 0){
			printf("Good bye!!!");
			break;

		}
		

		

	}



	


}