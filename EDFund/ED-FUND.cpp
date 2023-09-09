//*************************************************
//**  ED-FUND APPLICATION TO REGISTER :
//**     1. STUDENT FUND NEEDS
//**     2. SPONSORS DONATING TO SUPPORT THE NEEDS
//*************************************************

#include <fstream.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <process.h>
#include <iomanip.h>

//***************************************
//**  Declaration of File Names
//***************************************

#define summary_file   "SUMMARY.dat"
#define request_file   "FUNDREQ.dat"
#define sponsor_file   "SPONSOR.dat"

#define file_error_msg "\nERROR!FILE NOT FOUND"

//***************************************
//**  Structure to hold date
//***************************************
struct ddmmyyyy {
   int dd,mm,yyyy;
};

//**************************************
//** Structure to hold Summary details
//**************************************
struct summary {
   int   tot_students;
   int   tot_sponsors;
   int   tot_req;
   int   funded_req;
   int   open_req;
   float tot_req_amt;
   float tot_funded_amt;
   float tot_open_req_amt;
   float reserve_amt;
}s;

//**************************************
//** Class for managing Student Requests
//**************************************
class student {
   int      id;
   float    fund_needed;
   ddmmyyyy req_date;
   char     req_status;

  public:
  
   char name[20],course[20],phone[11];

   void add();
   void modify();
   void list();

   int ret_id() {
      return (id);
   }
   char ret_status() { //Return the value of the Status
        return (req_status);
   }
   float ret_fund_needed() { // Return Fund needed
      return (fund_needed);
   }
   ddmmyyyy ret_req_date(){
      return (req_date);
    }
   void change_status(char ch) { //Update Status
      req_status=ch;
   }
};

//**************************************
// Class to manage Sponsor details
//**************************************
class sponsor {
   int      id;
   float    amount;
   ddmmyyyy fund_date;
   char     fund_status;
   
  public:
  
   char name[20],phone[11];
   void add();
   void modify();
   void list();
};

//***************************************
//**  CHECK WHETHER GIVEN DATE IS VALID
//***************************************
int is_valid_date(ddmmyyyy dt){
   int month_days[12]={31,28,31,30,31,30,31,31,30,31,30,31};
   int leap=0;

   if (dt.yyyy %4 != 0) leap = 0; //leap year validatiion
   else if(dt.yyyy %100 != 0 )leap = 1;
   else if(dt.yyyy %400 == 0 ) leap = 1;

   if(leap == 1) month_days[1] = 29;
   if(dt.yyyy > 0 && (dt.mm >= 1 && dt.mm <= 12) && 
     (dt.dd >=1 && dt.dd <= month_days[dt.mm-1])) 
        return 1;
   else return 0;
}

//**************************************
//** Display new screen with sub_title
//**************************************
void display_title(char *sub_title){
   clrscr();
   gotoxy(15,2);cout<<"-------------------------------";
   gotoxy(18,3);cout<<"ED-FUND  ::  "<<sub_title;
   gotoxy(15,4);cout<<"-------------------------------";
}

//**************************************
//** Loads Summary info to global variable
//**************************************
void init() {
   ifstream fin;
   fin.open(summary_file,ios::binary|ios::in);
   fin.read((char*)&s,sizeof(s));
   fin.close();
}

//**************************************
//** Writes record to Summary file
//**************************************
void write_summary(){
   ofstream fout;
   fout.open(summary_file,ios::binary|ios::out);
   fout.write((char*)&s,sizeof(s));
   fout.close();
}

//**************************************
//** Available Reserve fund is utilized
//** to allocate to the open requests
//**************************************
void allocate_fund(){
   fstream file;
   file.open(request_file,ios::in|ios::out|ios::binary);
   if(!file){
      cout<<file_error_msg;
        exit(-1);
   }
   student tmp_st;
   while(file)  {
      file.read((char*)&tmp_st,sizeof(tmp_st));
      if( file.eof()) break;
      if( tmp_st.ret_status()=='O' && s.reserve_amt >= tmp_st.ret_fund_needed()){
         // Open requests with fund_needed < reserve fund
         s.reserve_amt-=tmp_st.ret_fund_needed();
         tmp_st.change_status('F');
         s.open_req--;
         s.funded_req++;
         s.tot_funded_amt+=tmp_st.ret_fund_needed();
         s.tot_open_req_amt-=tmp_st.ret_fund_needed();
         //re-write the record with updated status
         file.seekp((tmp_st.ret_id()-1)*sizeof(tmp_st),ios::beg);
         file.write((char*)&tmp_st,sizeof(tmp_st));
      }
   }
    file.close();
}
//**************************************
//** Register a new Student Request
//**************************************
void student::add(){
   display_title("ADD STUDENT");
   int y=7;
   gotoxy(10, y++);cout<<"ENTER STUDENT NAME              : ";gets(name);
   gotoxy(10, y++);cout<<"ENTER COURSE                    : ";gets(course);
   gotoxy(10, y++);cout<<"ENTER PHONE NUMBER              : ";gets(phone);
   gotoxy(10, y++);cout<<"ENTER REQD. AMOUNT              : ";cin>>fund_needed;
   while(fund_needed<=0){
      gotoxy(10, y++);cout<<"ENTER VALID REQD. AMOUNT ( > 0 ) : "; cin>>fund_needed;
   }

   gotoxy(10, y++);cout<<"ENTER REQUEST DATE (DD MM YYYY) : ";
   cin>>req_date.dd>>req_date.mm>>req_date.yyyy;
   while(!is_valid_date(req_date)){
      gotoxy(10, y++);cout<<"ENTER VALID DATE ( DD MM YYYY ) : ";
      cin>>req_date.dd>>req_date.mm>>req_date.yyyy;
   }
   id=++s.tot_students;
   s.tot_req++;
   s.open_req++;
   s.tot_req_amt+=fund_needed;
   s.tot_open_req_amt+=fund_needed;

   if(fund_needed > s.reserve_amt)
      req_status='O';
   else{
      req_status='F';
      s.reserve_amt-=fund_needed;
      s.tot_open_req_amt-=fund_needed;
      s.tot_funded_amt+=fund_needed;
      s.open_req--;
      s.funded_req++;
   }
   ofstream fout;
   fout.open(request_file,ios::binary|ios::app);
   if(!fout) {
      cout<<file_error_msg;
      exit(-1);
   }
   fout.write((char*)this,sizeof(student));
   fout.close();
   cout<<"\n         SUCCESSFULLY REGISTERED WITH ID  : "<<id;
   cout<<"\n         REQUEST STATUS                   : "<<req_status;
}

//**************************************
//** Modify existing Student Request
//**************************************
void student::modify(){
   display_title("MODIFY STUDENT");
   int y=6;
   gotoxy(15,y++);cout<<"Enter ID: "; cin>>id;
   fstream file;
   file.open(request_file,ios::binary|ios::in|ios::out);
   if(!file){
      cout<<file_error_msg;
      exit(-1);
   }
   student tmp;
   file.seekg((id-1)*sizeof(student));
   file.read((char*)&tmp,sizeof(tmp));
   if(id != tmp.ret_id()) {
      cout<<"ID does not exist! ";
      file.close();
      getch();
      return;
   }
   gotoxy(6, y++);cout<<"\tEXISTING DETAILS ARE:";
   gotoxy(10, y++);cout<<"NAME              : "<<tmp.name;
   gotoxy(10, y++);cout<<"COURSE            : "<<tmp.course;
   gotoxy(10, y++);cout<<"PHONE NUMBER      : "<<tmp.phone;
   gotoxy(10, y++);cout<<"REQUESTED AMOUNT  : "<<tmp.ret_fund_needed();
   gotoxy(10, y++);cout<<"REQUEST DATE      : "<<tmp.ret_req_date().dd<<' '
                       <<tmp.ret_req_date().mm<<' '<<tmp.ret_req_date().yyyy;
   gotoxy(10, y++);cout<<"REQUEST STATUS    : "<<tmp.ret_status();

   if(tmp.ret_status() == 'O') {
      gotoxy(6, y++);cout<<"ENTER NEW DETAILS: \n";
      gotoxy(10, y++);cout<<"NAME                         : ";gets(name);
      gotoxy(10, y++);cout<<"COURSE                       : ";gets(course);
      gotoxy(10, y++);cout<<"PHONE NUMBER                 : ";gets(phone);
      gotoxy(10, y++);cout<<"REQD. AMOUNT ( 0 to Cancel ) : ";cin>>fund_needed;

      while(fund_needed<0){
         gotoxy(10, y++);cout<<"ENTER VALID REQD. AMOUNT ( >= 0 ) : ";  
         cin>>fund_needed;
      }

      gotoxy(10, y++);cout<<"REQUEST DATE (DD MM YYYY)    : ";
      cin>>req_date.dd>>req_date.mm>>req_date.yyyy;
      while(!is_valid_date(req_date)){
         gotoxy(10, y++);cout<<"ENTER VALID DATE ( DD MM YYYY ) : ";
         cin>>req_date.dd>>req_date.mm>>req_date.yyyy;
      }
      s.tot_req_amt=s.tot_req_amt-tmp.fund_needed+fund_needed;
      s.tot_open_req_amt=s.tot_open_req_amt-tmp.fund_needed+fund_needed;
      if(fund_needed == 0) {
         tmp.req_status='C';
         s.tot_req--;
         s.open_req--;
      }
      else if(fund_needed > s.reserve_amt)
         tmp.req_status='O';
      else{
         tmp.req_status='F';
         s.reserve_amt-=fund_needed;
         s.tot_open_req_amt-=fund_needed;
         s.tot_funded_amt+=fund_needed;
         s.open_req--;
         s.funded_req++;
      }
      cout<<"\n\n\tSUCCESSFULLY UPDATED WITH STATUS : ";cout<<tmp.req_status;
      req_status=tmp.req_status;
      file.seekp((id-1)*sizeof(student));
      file.write((char*)this,sizeof(student));
   }
   else {
      cout<<"\n\n\tONLY OPEN REQUESTS CAN BE MODIFIED! ";
   }

   file.close();
}
//**************************************
//**  List all Student Requests
//**************************************
void student::list(){
   student tmp;
   fstream file;
   file.open(request_file,ios::binary|ios::in|ios::out);
   if(!file)    {
      cout<<file_error_msg;
      exit(-1);
   }
   file.seekg(0);
   int y = 0;

   while(!file.eof() )    {
      if (y > 23 ) { // Pause for user's view before scorlling
         getch(); y =0; // Reset number of lines printed on the screen to 0
      }
      if (y == 0) { // New page. Write Page header and column labels
         display_title("LIST STUDENTS");
         gotoxy(2,6);cout<<"ID";
         gotoxy(7,6);cout<<"NAME";
         gotoxy(29,6);cout<<"COURSE";
         gotoxy(51,6);cout<<"REQ DATE";
         gotoxy(65,6);cout<<"AMOUNT";
         gotoxy(73,6);cout<<"STATUS";
         gotoxy(1,7);for(int i=0;i<78;i++)cout<<'-';
         y=8;
      }

      file.read((char*)&tmp,sizeof(student));
      if(file.eof()) break;
   
      gotoxy(2,y);cout<<tmp.id;
      gotoxy(7,y);cout<<tmp.name;
      gotoxy(29,y);cout<<tmp.course;
      gotoxy(51,y);cout<<tmp.req_date.dd<<'-'<<tmp.req_date.mm<<'-'<<tmp.req_date.yyyy;
      gotoxy(63,y);cout<<setw(8)<<tmp.fund_needed;
      gotoxy(73,y);cout<<tmp.req_status;
      y++;
   }
   file.close();
}
//**************************************
//** Add a new sponsor info
//**************************************
void sponsor::add(){
   display_title("ADD SPONSOR");
   int y=7;
   gotoxy(10,y++);cout<<"ENTER SPONSOR NAME               : ";gets(name);
   gotoxy(10,y++);cout<<"ENTER PHONE NUMBER               : ";gets(phone);
   gotoxy(10,y++);cout<<"ENTER FUNDING AMOUNT ( > 0 )     : ";cin>>amount;
   while(amount<=0){
      gotoxy(10,y++);cout<<"ENTER VALID FUNDING AMOUNT ( > 0 ) : ";
      cin>>amount;
   }

	gotoxy(10,y++);cout<<"ENTER FUNDING DATE (DD MM YYYY)  : ";
   cin>>fund_date.dd>>fund_date.mm>>fund_date.yyyy;
   while(!is_valid_date(fund_date)){
      gotoxy(10,y++);cout<<"ENTER VALID DATE ( DD MM YYYY ) : ";
      cin>>fund_date.dd>>fund_date.mm>>fund_date.yyyy;
   }
   id=++s.tot_sponsors;
   s.reserve_amt+=amount;
   fund_status='F';
   allocate_fund();
   ofstream fout;
   fout.open(sponsor_file,ios::binary|ios::app);
   if(!fout){
      cout<<file_error_msg;
      exit(-1);
   }
   fout.write((char*)this,sizeof(sponsor));
   fout.close();
	cout<<"\n\tSUCCESSFULLY REGISTERED WITH ID : "<<id;
	cout<<"\n\tFUNDING STATUS                  : "<<fund_status;
}

//**************************************
//** Modify the sponsor info
//** Fund can be canceled only if sufficient reserve fund is available
//**************************************
void sponsor::modify(){
	display_title("MODIFY SPONSOR");
	int y=6;
	gotoxy(15,y++);cout<<"Enter SPONSOR ID: "; cin>>id;
	fstream file;
	file.open(sponsor_file,ios::binary|ios::in|ios::out);
	if(!file)    {
		cout<<file_error_msg;
		exit(-1);
	}
	sponsor tmp;
	file.seekg((id-1)*sizeof(sponsor));
	file.read((char*)&tmp,sizeof(tmp));
	if(id != tmp.id)
		cout<<"ID does not exist! ";
	else{
		gotoxy(6,y++);cout<<"EXISTING DETAILS ARE:"<<endl;
		gotoxy(10,y++);cout<<"NAME              : "<<tmp.name;
		gotoxy(10,y++);cout<<"PHONE NUMBER      : "<<tmp.phone;
		gotoxy(10,y++);cout<<"FUNDED AMOUNT     : "<<tmp.amount;
		gotoxy(10,y++);cout<<"FUNDING DATE      : "<<tmp.fund_date.dd
   		<<'-'<<tmp.fund_date.mm<<'-'<<tmp.fund_date.yyyy;
		gotoxy(10,y++);cout<<"FUND STATUS       : "<<tmp.fund_status;
		if (tmp.fund_status != 'F') {
			gotoxy(10,++y);cout<<"CAN MODIFY ONLY IF FUND STATUS = 'F'";
			file.close();
			getch();
			return;
		}
		gotoxy(6,y++);cout<<"NEW DETAILS: ";
		gotoxy(10,y++);cout<<"NAME                         : ";gets(name);
		gotoxy(10,y++);cout<<"PHONE NUMBER                 : ";gets(phone);
		gotoxy(10,y++);cout<<"FUNDING AMOUNT (0 to Cancel) : ";cin>>amount;
		while(amount<0){
		   gotoxy(10,y++);cout<<"ENTER VALID FUNDING AMOUNT ( >= 0 ) : ";
         cin>>amount;
		}

		gotoxy(10,y++);cout<<"FUNDING DATE (DD MM YYYY)    : ";
		cin>>fund_date.dd>>fund_date.mm>>fund_date.yyyy;

		while(!is_valid_date(fund_date)){
			gotoxy(10,y++);cout<<"ENTER VALID DATE ( DD MM YYYY ) : ";
			cin>>fund_date.dd>>fund_date.mm>>fund_date.yyyy;
		}
		if(s.reserve_amt < tmp.amount-amount){
			cout<<"\n\n      ERROR! NOT ENOUGH RESERVE FUND TO MODIFY ";
		}
		else if(amount < 0){
			cout<<"\n\n       ERROR! AMOUNT SHOULD BE >= 0 ";
		}
		else{
			if(amount !=0) {
				s.reserve_amt=s.reserve_amt-tmp.amount+amount;
			}
			else{
				tmp.fund_status='C';
				s.reserve_amt=s.reserve_amt-tmp.amount+amount;
			}
			fund_status=tmp.fund_status;
			file.seekp((id-1)*sizeof(sponsor));
			file.write((char*)this,sizeof(sponsor));

			cout<<"\n\n      SUCCESSFULLY UPDATED WITH STATUS : ";cout<<tmp.fund_status;
			allocate_fund();
		}
	}
   file.close();
}
//**************************************
//**  List all Sponsors
//**************************************
void sponsor::list(){

   sponsor tmp;
   fstream file;
   file.open(sponsor_file,ios::binary|ios::in|ios::out);
   if(!file)   {
      cout<<file_error_msg;
      exit(-1);
   }
   file.seekg(0);
   int y=0;
   while(!file.eof() )    {
      if ( y > 23 ) {
         getch();
         y =0;
      }
      if ( y == 0) {
         display_title("LIST SPONSORS");
         gotoxy(2,6);cout<<"ID";
         gotoxy(7,6);cout<<"NAME";
         gotoxy(31,6);cout<<"REQ DATE";
         gotoxy(45,6);cout<<"AMOUNT";
         gotoxy(53,6);cout<<"STATUS";
         gotoxy(1,7);for(int i=0;i<78;i++)cout<<'-';
         y = 8;
      }
      file.read((char*)&tmp,sizeof(sponsor));
      if(file.eof()) break;

      gotoxy(2,y);cout<<tmp.id;
      gotoxy(7,y);cout<<tmp.name;
      gotoxy(31,y);cout<<tmp.fund_date.dd<<'-'
         <<tmp.fund_date.mm<<'-'<<tmp.fund_date.yyyy;
      gotoxy(43,y);cout<<setw(8)<<tmp.amount;
      gotoxy(53,y);cout<<tmp.fund_status;
      y++;
   }
   file.close();
}

//**************************************
//** DISPLAY SUMMARY INFORMATION
//**************************************
void display_summary() {
   display_title("SUMMARY");
   gotoxy(10,  6);cout<<"TOTAL NUMBER OF REQUESTS (INCL. CANCELED) : "
                      <<setw(8)<<s.tot_students;
   gotoxy(10,  7);cout<<"TOTAL NUMBER OF REQUESTS (EXCL. CANCELED) : "
                      <<setw(8)<<s.tot_req;
   gotoxy(10,  8);cout<<"TOTAL NUMBER OF FUNDED REQUESTS           : "
                      <<setw(8)<<s.funded_req;
   gotoxy(10,  9);cout<<"TOTAL NUMBER OF OPEN REQUESTS             : "
                      <<setw(8)<<s.open_req;
   gotoxy(10, 10);cout<<"TOTAL NUMBER OF SPONSORS                  : "
                      <<setw(8)<<s.tot_sponsors;
   gotoxy(10, 12);cout<<"TOTAL REQUESTED AMOUNT                    : "
                      <<setw(8)<<s.tot_req_amt;
   gotoxy(10, 13);cout<<"TOTAL FUNDED AMOUNT                       : "
                      <<setw(8)<<s.tot_funded_amt;
   gotoxy(10, 14);
   if(s.reserve_amt < s.tot_open_req_amt)
      cout<<"TOTAL AMOUNT NEEDED                       : "
         <<setw(8)<<s.tot_open_req_amt-s.reserve_amt;
   else
      cout<<"TOTAL AMOUNT NEEDED                       : "
         <<setw(8)<<"-";
   gotoxy(10,15);cout<<"TOTAL RESERVE AMOUNT                      : "
         <<setw(8)<<s.reserve_amt;
}


//**************************************
//** MAIN MENU
//**************************************
void main()
{
   char choice;
   char ch;
   student stu;
   sponsor sp;

   cin.setf(ios::dec); // Set input numerical data as decimal
   init();             // Call init() to initialize

   do{
      display_title("MAIN MENU");
      gotoxy(10, 6);cout<<"A. STUDENT DETAILS";
      gotoxy(14, 7);cout<<"1. ADD";
      gotoxy(14, 8);cout<<"2. MODIFY";
      gotoxy(14, 9);cout<<"3. LIST";
      gotoxy(10,10);cout<<"B. SPONSOR DETAILS";
      gotoxy(14,11);cout<<"4. ADD";
      gotoxy(14,12);cout<<"5. MODIFY";
      gotoxy(14,13);cout<<"6. LIST";
      gotoxy(10,14);cout<<"C. GENERIC";
      gotoxy(14,15);cout<<"7. SUMMARY";
      gotoxy(14,16);cout<<"8. EXIT";
      gotoxy(10,18);cout<<"ENTER YOUR CHOICE (between 1 and 8): ";

   enter_choice:

      cin>>choice;

      switch(choice){
         case '1':
            stu.add();
            break;
         case '2':
            stu.modify();
            break;
         case '3':
            stu.list();
            break;
         case '4':
            sp.add();
            break;
         case '5':
            sp.modify();
            break;
         case '6':
            sp.list();
            break;
         case '7':
            display_summary();
            break;
         case '8':
            cout<<"\n ******************************************************* ";
            cout<<"\n **                                                   ** ";
            cout<<"\n **    THANK YOU FOR USING ED-FUND APPLICATION  !!    ** ";
            cout<<"\n **                                                   ** ";
            cout<<"\n ******************************************************* ";
            getch();
            return;
         default:
            cout<<"Invalid choice. Please enter again :";
            goto enter_choice;
      }
      write_summary();
        cout<<"\n\n    DO YOU WANT TO RETURN TO MAIN MENU? (Y/N): ";cin>>ch;
   }while(ch=='Y' || ch=='y');
}

//**** END OF PROGRAM *****
