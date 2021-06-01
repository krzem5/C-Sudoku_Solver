#include <sudoku.h>
#include <intrin.h>
#include <stdint.h>
#pragma intrinsic(__popcnt16)
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanForward64)



#define Z81_MASK 0x8000



typedef struct __SOLVE_BOARD{
	uint64_t z64;
	uint16_t z80;
	uint16_t dt[27];
} solve_board_t;



uint8_t _solve(solve_board_t* b,uint8_t* o){
	unsigned short f;
	uint8_t nmi;
	uint32_t z81=(b->dt[0]&Z81_MASK);
	b->dt[0]&=~Z81_MASK;
	do{
		f=10;
		uint64_t z64=b->z64;
		uint32_t z32=b->z80|(z81<<1);
		while (z64||z32){
			unsigned long i;
			if (z64){
				_BitScanForward64(&i,z64);
				z64&=~(1ull<<i);
			}
			else{
				_BitScanForward(&i,z32);
				z32&=~(1u<<i);
				i+=64;
			}
			uint16_t* j=b->dt+i/9;
			uint16_t* k=b->dt+i%9+9;
			uint16_t* l=b->dt+i/27*3+(i%9)/3+18;
			uint16_t s=(*j)&(*k)&(*l);
			if (!s){
				return 0;
			}
			unsigned short bc=__popcnt16(s);
			if (bc==1){
				unsigned long bi;
				_BitScanForward(&bi,s);
				*(o+i)=(uint8_t)bi+1;
				if (i<64){
					b->z64&=~(1ull<<i);
				}
				else if (i<80){
					b->z80&=~(1u<<(i-64));
				}
				else{
					z81=0;
				}
				(*j)&=~s;
				(*k)&=~s;
				(*l)&=~s;
				f=0;
			}
			else if (f&&bc<f){
				f=bc;
				nmi=(uint8_t)i;
			}
		}
	} while (!f);
	if (f==10){
		return 1;
	}
	if (nmi<64){
		b->z64&=~(1ull<<nmi);
	}
	else if (nmi<80){
		b->z80&=~(1u<<(nmi-64));
	}
	else{
		b->dt[0]&=~Z81_MASK;
	}
	uint8_t j=(nmi)/9;
	uint8_t k=(nmi)%9+9;
	uint8_t l=j/3*3+k/3+15;
	uint16_t s=(b->dt[j])&(b->dt[k])&(b->dt[l]);
	b->dt[0]|=z81;
	solve_board_t nb=*b;
	uint16_t* nb_j=nb.dt+j;
	uint16_t* nb_k=nb.dt+k;
	uint16_t* nb_l=nb.dt+l;
	while (1){
		unsigned long i;
		_BitScanForward(&i,s);
		uint16_t m=~(1<<(uint16_t)i);
		s&=m;
		(*nb_j)&=m;
		(*nb_k)&=m;
		(*nb_l)&=m;
		if (_solve(&nb,o)){
			*(o+nmi)=(uint8_t)i+1;
			return 1;
		}
		if (!s){
			return 0;
		}
		nb=*b;
	}
}



uint8_t solve_sudoku(uint8_t* b){
	solve_board_t sb={
		0,
		0,
		{
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff,
			0x1ff
		}
	};
	for (uint8_t i=0;i<81;i++){
		if (*(b+i)){
			uint16_t m=~(1<<((*(b+i))-1));
			sb.dt[i/9]&=m;
			sb.dt[i%9+9]&=m;
			sb.dt[i/27*3+(i%9)/3+18]&=m;
		}
		else{
			if (i<64){
				sb.z64|=(1ull<<i);
			}
			else if (i<80){
				sb.z80|=(1<<(i-64));
			}
			else{
				sb.dt[0]|=Z81_MASK;
			}
		}
	}
	return _solve(&sb,b);
}
