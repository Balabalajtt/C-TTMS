#include "Ticket_UI.h"

#include "../Common/list.h"
#include "../Service/ticket.h"
#include "../Service/schedule.h"
#include "../Service/play.h"
#include "../Service/studio.h"

#include <stdio.h>
#include <stdlib.h>

//����״̬���ض�Ӧ��ʾ״̬����
char Ticket_UI_Status2Char(ticket_status_t status) {

	char statusChar;
	switch (status) {
	case TICKET_AVL:		//����λ
		statusChar = '#';
		break;
	case TICKET_SOLD:	//�𻵵���λ
		statusChar = '@';
		break;
	case TICKET_RESV:
	default:
		statusChar = '~';
		break;
	}
	return statusChar;
}

void Ticket_UI_Print(int ID){
	//����ID����Ʊ
	ticket_t ticket;
	if(!Ticket_Srv_FetchByID(ID, &ticket)){
		printf("\t\tû�и�Ʊ \n��[Enter]������\n ");
		return ;
	}

	//�����ݼ��ƻ�����Ŀ
	schedule_t sch;
	play_t play;
	Schedule_Srv_FetchByID(ticket.schedule_id, &sch);
	Play_Srv_FetchByID(sch.play_id, &play);

	//������λ���ݳ���
	seat_t seat;
	studio_t room;
	Seat_Srv_FetchByID(ticket.seat_id, &seat);
	Studio_Srv_FetchByID(seat.roomID, &room);

	//��ʾƱ��Ϣ
	printf("\n--------------------------------------------------\n");
	printf("|%9s%-10d%15s%-5d%5s%-4d|\n", "Ticket ID:", ticket.id,
			"Row:", seat.row, "Col:", seat.column );
	printf("|%9s%-39s|\n", "Play Name:", play.name);
	printf("|%9s%-11d %16d-%2d-%2d %2d:%2d|\n", "Price:", play.price,
				sch.date.year, sch.date.month, sch.date.day,
				sch.time.hour, sch.time.minute);
	printf("--------------------------------------------------\n");
}

//���ݼƻ�ID��ʾ����Ʊ
void Ticket_UI_ListBySch(const schedule_t *sch,	ticket_list_t tickList, seat_list_t seatList) {
	assert(NULL!=sch && NULL!=tickList && NULL!=seatList);
	int i, j;

	studio_t studioRec;
	play_t   playRec;
	ticket_node_t *pTicket;
	seat_node_t *pSeat;

	if (!Studio_Srv_FetchByID(sch->studio_id, &studioRec)) {  //��ö�Ӧid��ӳ������Ϣ
		printf("\t\t\t���ݳ�����������!\n��[Enter]������!\n");
		getchar();
		return;
	}

	if (!Play_Srv_FetchByID(sch->play_id, &playRec)) {  //��ö�Ӧid��Ŀ����Ϣ
		printf("\t\t\t�þ�Ŀ��������!\n��[Enter]������!\n");
		getchar();
		return;
	}

	printf( "********************** Ticket List for %s ***********************\n", playRec.name);
	printf("%5c", ' ');
	for (i = 1; i <= studioRec.colsCount; i++) {
		printf("%3d", i);
	}

	printf("\n------------------------------------------------------------------\n");
	//��ʾ����
	for (i = 1; i <= studioRec.rowsCount; i++) {
		j = 1;
		printf("%2d:%c", i, '|');
		List_ForEach(tickList, pTicket)
		{
			pSeat=Seat_Srv_FindByID(seatList, pTicket->data.seat_id);
			if(NULL!=pSeat && pSeat->data.row == i) {
				while (pSeat->data.column != j) {
					printf("%3c", ' ');
					j++;
				}
				printf("%3c", Ticket_UI_Status2Char(pTicket->data.status));
				j++;
			}
		}
		printf("\n");
	}
	printf("******************************************************************\n");
}

void ListTickets(void){
	int i, schedule_id;
	schedule_t schedule_rec;
	play_t play_rec;
	char choice;

	ticket_list_t head;
	ticket_node_t *pos;
	Pagination_t paging;

	List_Init(head, ticket_node_t);
	paging.offset = 0;
	paging.pageSize = TICKET_PAGE_SIZE;

	printf("�������ݳ��ƻ�ID!\n");
	scanf("%d",&schedule_id);

	//��������
	paging.totalRecords = Ticket_Srv_FetchBySchID(head,schedule_id);
	Paging_Locate_FirstPage(head, paging);
	//��Ҫ���Ӳ�����λ��Ϣ

	Schedule_Srv_FetchByID(schedule_id,&schedule_rec);
	Play_Srv_FetchByID(schedule_rec.play_id,&play_rec);
	do {
		printf("\n\t\t�����������������������������������������������\n");
			printf("\n\t\t\t\t\tƱ����Ϣ�б�\n");
			printf("\n\t\t�����������������������������������������������\n");
			printf("\n");
			printf("%-5s %-30s %-10s %-5s %-5s %-6s %-6s %-8s %-8s\n","ID","��Ŀ��","����","ʱ��","Ʊ��",
							"��Ŀ����","��Ŀ����","��ӳʱ��","��ӳʱ��");
			printf("-------------------------------------------------------\n");
		//��ʾ����
		for (i = 0, pos = (ticket_node_t *) (paging.curPos);
				pos != head && i < paging.pageSize; i++) {
			printf("%d\t%s\t%d-%d-%d\t%d:%d\t%d\t%s\n"/*t%d\t%d*/, pos->data.id, play_rec.name,
//					seat_rec.row, seat_rec.column,
					schedule_rec.date.year,schedule_rec.date.month,
					schedule_rec.date.day,schedule_rec.time.hour,
					schedule_rec.time.minute,pos->data.price,
					pos->data.status==0?"to sell":"sold");
			pos = pos->next;
		}
		printf(
				"== Total Records:%d =========================== Page %d/%d ==\n",
				paging.totalRecords, Pageing_CurPage(paging),
				Pageing_TotalPages(paging));
		//printf("[P]revPage|[N]extPage | [U]pdate | [R]eturn ");
		printf("\t\t[P]��һҳ\t[N]��һҳ\t [R]���� ");

		fflush(stdin);
		scanf("%c", &choice);
		switch (choice) {
	/*	case 'u':
		case 'U':
			printf("\t\t������Ʊ��ID:");
			scanf("%d", &id);
			if (UpdateTicket(id)) {	//������������
				paging.totalRecords = Ticket_Srv_FetchBySchID(head,schedule_rec.id);
				List_Paging(head, paging, ticket_node_t);
			}
			break;
	*/	case 'p':
		case 'P':
			if (1 < Pageing_CurPage(paging)) {
				Paging_Locate_OffsetPage(head, paging, -1, ticket_node_t);
			}
			break;
		case 'n':
		case 'N':
			if (Pageing_TotalPages(paging) > Pageing_CurPage(paging)) {
				Paging_Locate_OffsetPage(head, paging, 1, ticket_node_t);
			}
			break;
		}
	} while (choice != 'r' && choice != 'R');
	//�ͷ������ռ�
	List_Destroy(head, ticket_node_t);
	}


int UpdateTicket(int id){
	ticket_t rec;
	schedule_t schedule_rec;
	play_t play_rec;
	int rtn = 0;


	/*Load record*/
	if (!Ticket_Srv_FetchByID(id, &rec)) {
		printf("�þ�Ŀ������!\n�� [Enter]������!\n");
		getchar();
		return 0;
	}
	Schedule_Srv_FetchByID(rec.schedule_id,&schedule_rec);
	Play_Srv_FetchByID(schedule_rec.play_id,&play_rec);
	//��Ҫ���Ӳ�����λ��Ϣ

	 printf("\n\t\t�����������������������������������������������\n");
			printf("\n\t\t\t\t\t����Ʊ����Ϣ\n");
			printf("\n\t\t�����������������������������������������������\n");
			printf("\t\t\tƱ��ID:%d\n", rec.id);
			printf("\t\t\t��Ŀ����[%s]:", play_rec.name);
	//��Ҫ�����λ�����к�
	printf("Schedule date(yyyy-mm-dd)[%d-%d-%d]:",schedule_rec.date.year,
			schedule_rec.date.month,schedule_rec.date.day);
	printf("Schedule time[%d:%d]:",schedule_rec.time.hour,schedule_rec.time.minute);
	printf("Ticket price[%d]:",rec.price);
	scanf("%d",&(rec.price));
	printf("Ticket status[%s](0.to sell,1.sold):",rec.status==1?"to sell":"sold");
	scanf("%d",&rec.status);
	printf("-------------------------------------------------------\n");

	if (Ticket_Srv_Modify(&rec)) {
		rtn = 1;
		printf(
				"\t\t\tƱ����Ϣ���³ɹ���!\n��[Enter]������!\n");
					} else
						printf("\t\t\tƱ����Ϣ����ʧ��!\n��[Enter]������!\n");

	getchar();
	return rtn;
}


int QueryTicket(int id){

	int rtn=0;
	schedule_t schedule_rec;
	play_t play_rec;
	ticket_t rec;
	if (Ticket_Srv_FetchByID(id,&rec)) {
		printf("\n\t\t�����������������������������������������������\n");
				printf("\n\t\t\t\t\tƱ����Ϣ\n");
				printf("\n\t\t�����������������������������������������������\n");

				printf("\tID\t\tPlay Name\t\tSeat Row\tSeat Col\t Date\tTime\t\tPrice\tStatus\n");
				printf("-------------------------------------------------------\n");

		Schedule_Srv_FetchByID(id,&schedule_rec);
		Play_Srv_FetchByID(schedule_rec.play_id,&play_rec);
		//���Ӳ�����λ������Ϣ�ĺ���
		printf("%d\t%s\\t%d-%d-%d\t%d:%d\t%d\t%d\n", rec.id, play_rec.name,

				schedule_rec.date.year,schedule_rec.date.month,
				schedule_rec.date.day,schedule_rec.time.hour,
				schedule_rec.time.minute,rec.price,rec.status);
		rtn=1;
	}
	else
	{
		printf("\t\t\t����������Ʊ!\n��[Enter]������!\n");
	}
	printf("-------------------------------------------------------\n");

	system("pause");
	return rtn;
}