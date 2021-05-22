/*
 * CFile1.c
 *
 * Created: 20/05/2021 21:28:30
 *  Author: Gabi
 */ 
#include "defs.h"
#include "nokia5110.h"

uint8_t FreqRespiracao = 15, bpm, sel = 0, sel2 = 0, O2 = 0, VolAr = 1, l = 0, t = 0, flag, flag_sat; //variavel de controle dos bpm
char b[3], c[4], sp02[4], temp[5], O21[4], Vol[2], pH[5];
char pressao[10]; //b-> resp/min c-> bpm, saturacao de oxigenio e temperatura
uint32_t tempo_ms = 0, tempo2 = 0, tempobpm1=0, tempobpm2=0, tempo150=0, tempo200=0, tempo_sat =1;
uint16_t leitura2, leitura3, conv2, conv1;
float leitura1, conv, conv3;
unsigned int truth=2, r = 0;

void enabling(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr>>8); 
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (0<<USBS0)|(3<<UCSZ00); //primeiro é o de parada e dps o tamanho da transmissao 
}
unsigned char USART_Recepcao()
{
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;  
}
void USART_Transmit(unsigned char data)
{
    while(!( UCSR0A & (1<<UDRE0)));//Espera a limpeza do registr. de transmissão
    UDR0 = data; //Coloca o dado no registrador e o envia
}
void limita ()
{
	if ((FreqRespiracao) > 30)
	 FreqRespiracao = 30; 
	else if ((FreqRespiracao) < 5)
	{
	 FreqRespiracao = 5;
	}
}

void limitaO2(uint8_t *O)
{
	if(*O>10)
		*O=10;
}
//gera o tempo do periodo
double timing_google ()
{
	return 60000.0/(FreqRespiracao*16); 
	//utilizei 60000 segundos pq quando for passar pra ms não terá alteração no valor
}

void pressure()
{
	char vect[8];
	vect[7] = '\0';
	
	nokia_lcd_set_cursor(0,40);
	for(int p=0; p<7; p++)
	{
		vect[p] = pressao[p+1];
	}
	if(truth == 1)
	{
		nokia_lcd_write_string(vect, 1);
	}
	else if(truth == 2)
	{
		nokia_lcd_write_string("HHHxLLL", 1);
	}
	else
	{
		nokia_lcd_write_string("Erro!", 1);
	}
	nokia_lcd_set_cursor(45, 40);
	nokia_lcd_write_string("mmHg", 1);
}
void conversao ()
{
	flag = 0; 
	b[1] = (FreqRespiracao%10) + '0';
	b[0] = (FreqRespiracao/10) + '0';
	
	O21[2] = ((O2*10)%100)%10 + '0';
	O21[1] = ((O2*10)%100)/10 + '0'; 
	O21[0] = ((O2*10)/100) + '0'; 
	
	c[2] = (bpm%100)%10 + '0';
	c[1] = (bpm%100)/10 + '0';
	c[0] = (bpm/100) + '0'; 
	
	sp02[2] = (conv2%100)%10 + '0';
	sp02[1] = (conv2%100)/10 + '0';
	sp02[0] = (conv2/100) + '0'; 
	
	pH[4] = '\0';
	pH[3] = ((int)(conv3*100)%100)%10 + '0';
	pH[2] = ((int)(conv3*100)%100)/10 + '0';
	pH[1] = 46;
	pH[0] = ((int)(conv3*100)/100) + '0'; 
 
	conv1 = (int)(conv*10);
	
	temp[3] = (conv1%100)%10 + '0';
	temp[2] = 46;// ponto
	temp[1] = (conv1%100)/10 + '0';
	temp[0] = (conv1/100) + '0'; 
	
	Vol[0] = VolAr%10 + '0'; 
	
	if((tempo_ms - tempo200) >= 200)//a cada 200ms, é mostrada uma atualizacao no display. 
	{
		if(sel == 0)//sinais vitais
		{
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0); 
			nokia_lcd_write_string("Sinais Vitais", 1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string(c, 1);
			nokia_lcd_set_cursor(30, 10);
			nokia_lcd_write_string("bpm", 1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string(sp02, 1);
			nokia_lcd_set_cursor(30, 20);
			nokia_lcd_write_string("SpO2", 1);
			nokia_lcd_set_cursor(0,30);
			nokia_lcd_write_string(temp, 1);
			nokia_lcd_set_cursor(30, 30);
			nokia_lcd_write_string("C", 1);
			pressure();
			nokia_lcd_render();
		}
		else if(sel == 2)
		{
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("Sinais Vitais", 1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string(sp02, 1);
			nokia_lcd_set_cursor(30, 20);
			nokia_lcd_write_string("SpO2", 1);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string(pH, 1);
			if((conv3 > 6.85) | (conv3 < 7.35) | (conv3 > 7.45) | (conv3 < 7.95) | (conv3 < 6.85) | (conv3 > 7.8))
			{
				flag=1;
				nokia_lcd_set_cursor(30, 30);
				nokia_lcd_write_string("pH", 1);
			}
			else 
			{
				flag = 0; 
			}
			nokia_lcd_render();
		}
		else if(sel == 4)//mostra a freq resp
		{
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0); 
			nokia_lcd_write_string("Parametros", 1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string(b, 1);
			nokia_lcd_set_cursor(30, 10);
			nokia_lcd_write_string("* resp/min", 1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string(O21, 1);
			nokia_lcd_set_cursor(30, 20);
			nokia_lcd_write_string("%O2", 1);
			nokia_lcd_set_cursor(0,30);
			nokia_lcd_write_string(Vol, 1);
			nokia_lcd_set_cursor(30, 30);
			nokia_lcd_write_string("Vol", 1);
			nokia_lcd_render(); 
		}
		else if(sel == 6)//mostra a freq resp
		{
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0); 
			nokia_lcd_write_string("Parametros", 1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string(b, 1);
			nokia_lcd_set_cursor(30, 10);
			nokia_lcd_write_string("resp/min", 1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string(O21, 1);
			nokia_lcd_set_cursor(30, 20);
			nokia_lcd_write_string("* %O2", 1);
			nokia_lcd_set_cursor(0,30);
			nokia_lcd_write_string(Vol, 1);
			nokia_lcd_set_cursor(30, 30);
			nokia_lcd_write_string("Vol", 1);
			nokia_lcd_render(); 
		}
		else
		{
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0); 
			nokia_lcd_write_string("Parametros", 1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string(b, 1);
			nokia_lcd_set_cursor(30, 10);
			nokia_lcd_write_string("resp/min", 1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string(O21, 1);
			nokia_lcd_set_cursor(30, 20);
			nokia_lcd_write_string("%O2", 1);
			nokia_lcd_set_cursor(0,30);
			nokia_lcd_write_string(Vol, 1);
			nokia_lcd_set_cursor(30, 30);
			nokia_lcd_write_string("* Vol", 1);
			nokia_lcd_render(); 
		}
	tempo200 = tempo_ms;
	}	
}
void buzzer ()//funcao do buzzer
{
	if ((conv > 41.0) | (conv < 35.0) | (conv2 < 60) | (OCR1A == 2000)| (flag==1))
	{
		PORTD = 0b11111111;   
	}
	else 
	{
		PORTD = 0b01111111; 
	}
	
}
void funcao_led (int *a)
{
	int static i=1; 
	double static tempo;
	tempo = timing_google(FreqRespiracao);
		if(i==2)
		{
			i=1;
			*a = !(*a); //nega o valor de a pra chegar na proxima interacao do for. 
		} //chamei a funcao de conversao aqui pq ficaria mais rapido, mas só vai printar a cada 200 ms
		conversao(); 
		if ((tempo_ms - tempo2) >= tempo*8) //logica de pseudothread
		{
	
			if (*a)
			{
				OCR1A = 250*i*VolAr + 2000; // equacao da reta dos pontos
			}
			else
			{
				OCR1A -= 250*i*VolAr; 
			}
			tempo2 = tempo_ms; 
			i++;
			buzzer(); 
		}
}
//Funções de interrupção para os botoes INT0, INT1 e timer 
ISR(TIMER0_COMPA_vect)
{
	tempo_ms += 1; 
}

void func_servo()//essa funcao abaixo contra o movimento do servo de O2
{
	OCR1B = 200*O2 +2000;
}
ISR(PCINT2_vect) 
{
	if (l == 0)
	{
		tempobpm1 = tempo_ms; 
		l++;
	}
	else if (l == 1)
	{
		tempobpm2 = tempo_ms; 
		l = 0;
		bpm = 30000./((tempobpm2-tempobpm1));
	}
}
//faço mais conversoes aqui tbm pq o valor de Freq é atualizado aqui tbm
ISR(INT0_vect)
{
	if((sel == 2) | (sel == 4))
	{
	FreqRespiracao +=1; 
	limita(&FreqRespiracao);
	}
	else if(sel == 6)
	{ 
		if(((conv2 < 85) | (conv3 < 7.35)) && (sel2 >= 2)) //momento em que a respiracao do paciente começa a ser prejudicada
		{
			if((tempo_ms - tempo_sat) > tempo_sat*1000)
				O2 += 1; //aumenta a quantidade de oxigenio que o paciente recebe
			else
			{
				O2+=0;
			}
			
			if(tempo_sat > 7200000) //se tempo_sat for maior de 2hs, uma flag será ativada, pois não houve melhora no quadro do paciente e algo deve ser visto pela equpe médica
				flag_sat = 1;
			else
			{
				flag_sat = 0; 
			}
			tempo_sat = tempo_ms;
		}	
		O2+= 1;
		limitaO2(&O2);
		func_servo();
		//chama funcao do servo do O2
	}
	else if(sel == 8)
	{
		VolAr += 1;
		if(VolAr>8)
			VolAr = 8;
	}
}
ISR(INT1_vect)
{
	
	if(sel == 2 || sel == 4)
	{
	FreqRespiracao -= 1; 
	limita(&FreqRespiracao);
	}
	else if(sel == 6)
	{
		if(O2>0)
		O2-= 1;
		limitaO2(&O2);
		func_servo();
		//chama funcao do servo O2 
	}
	else if(sel == 8)
	{
		VolAr -= 1;
		if(VolAr<1)
			VolAr = 1;
	}
}
ISR(ADC_vect)
{
	if ((tempo_ms-tempo150)>=150)
	{
		if (t==0)//sensor C0
		{	
		ADMUX = 0b01000000;
		DIDR0 = 0b00111110;
		leitura1 = ADC;
		t++;
		if(leitura1 <= 410)
		{
			conv = 30; 
		}
		else if(leitura1 >= 716 )
		{
			conv = 45; 
		}
		else
		{
			conv = (leitura1 + 205)/20.5; /*aqui eu relacionei a funcao da reta da temperatura com o numero de 'a' pontos que seria lido pelo adc,
			dai relacionei 5 volts com 1024 pontos, como se fosse uma regra de 3 e obtive uma funcao que relaciona a quantidade de pontos com a temperatura
			no caso, meu x: 
			0.1x - 1 -- a
			5 -- 1024, isolei o x em funcao de 'a' e deu isso aí.*/ 
		}

		}
		else if (t==1)//sensor C1
		{
		ADMUX = 0b01000001;
        DIDR0 = 0b00111101;
		t++;
		leitura2 = ADC;
		conv2 = (int)leitura2*100.0/819.0; //aqui para o sensor de saturacao de oxigenio 
		
		}
		else
		{
		ADMUX = 0b01000010;
		DIDR0 = 0b00111011; 
		t=0;
		leitura3 = ADC;
		conv3 = (leitura3*(1.1/1024) + 6.85);
		
		}
	tempo150 = tempo_ms;
	}
}
ISR(PCINT1_vect)
{
	sel2+=1; 
	USART_Transmit(sel2 + '0');
	if(sel2>=4)
	sel2=0;
}
//vendo a chave seletora
ISR(PCINT0_vect)
{
	sel += 1; 
	
	if(sel == 10)
	{
		sel = 0; 
	}	
}
//fiz o tratamento de erros aqui
ISR(USART_RX_vect)
{
	pressao[r] = USART_Recepcao(); 

	if (r==8)
	{
		truth = 1; 
		if ((pressao[0] != ';')|(pressao[8] != ':')|(pressao[4] !='x'))
				truth = 0; 
		else{	
			for(int u = 1; u<4; u++)
			{	
				if((pressao[u] - '0') > 9 )
					truth =0; 
			}	
			for(int u =5; u < 8; u++)
		{
			if((pressao[u] - '0') > 9 )
				truth =0; 
		}
		}

	r = 0;
	}
	else
	{
	r++; 
	}
}
