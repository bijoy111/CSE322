#include<bits/stdc++.h>
#include <windows.h>
using namespace std;

string compute_crc_checksum(string frame, string gp)
{
    int r = gp.length() - 1; // Degree of generator polynomial
    int m = frame.length(); // Length of the frame
    // Append r zero bits to the low-order end of the frame
    frame.append(r, '0');
    // Perform polynomial division
    for (int i = 0; i < m; i++)
    {
        if (frame[i] == '1')
        {
            // Perform modulo 2 subtraction
            for (int j = 0; j <= r; j++)
            {
                frame[i + j] = (frame[i + j] == gp[j]) ? '0' : '1';
            }
        }
    }
    // The result is the checksummed frame (T(x))
    return frame.substr(m);
}

int verify_received_frame(string receivedFrame, string gp)
{
    string remainder = compute_crc_checksum(receivedFrame, gp);
    // If the remainder is all zeros, the frame is valid
    return remainder == string(remainder.length(), '0');
}

int main()
{
    cout<<"enter data string:";
    string str;
    getline(cin, str);
    cout<<"enter number of data bytes in a row (m):";
    int m;
    cin>>m;
    cout<<"enter probability (p):";
    double p;
    cin>>p;
    cout<<"enter generator polynomial:";
    string g;
    cin>>g;
    //step 1 started
    int l=str.length();
    while(l%m!=0)
    {
        char ch='~';
        str+=ch;
        l=str.length();
    }
    cout<<endl<<endl<<"data string after padding: "<<str<<endl<<endl;
    //step 1 ended

    //step 2 started
    cout<<"data block (ascii code of m characters per row): ";
    int row=str.length()/m;
    int col=m*8;
    vector<string> data_block;
    for(int i=0; i<str.length();)
    {
        string tmp;
        for(int j=1; j<=m; j++)
        {
            bitset<8> binaryValue(str[i]);
            if(j==1)
                tmp=binaryValue.to_string();
            else
                tmp+=binaryValue.to_string();
            i++;
        }
        data_block.push_back(tmp);
    }
    cout<<endl;
    for(int i=0; i<data_block.size(); i++)
        cout<<data_block[i]<<endl;
    //step 2 ended

    //step 3 started
    cout<<endl<<"data block after adding check bits: "<<endl;
    int data_len=m*8;
    int r;
    for(int i=0; (data_len+1)>(pow(2,i)-i); i++)
        r=i;
    r++;
    col+=r;
    vector<string> data_block1;
    for(int i=0; i<data_block.size(); i++)
    {
        int len1=data_len+r;
        string str1(len1,'0'); // str1 is a string of length len1(data_len+r) initialize with 0 at each index
        for(int j=0,k=0,j1=0; j<len1; j++)
        {
            if(j==(pow(2,j1)-1))
                j1++;
            else
            {
                str1[j]=data_block[i][k];
                k++;
            }
        }
        // now i have a string named str1 which contain all data bit at corresponding position and contain 0 at 2^i-1 (i>=0) postition
        for(int j=0,j1=0; j<len1; j++)
        {
            if(j==(pow(2,j1)-1))
            {
                j1++;
                continue;
            }
            int k=j+1; // as string is 0 indexing so to match with our formula of book we make it 1 indexing
            while(k!=0)
            {
                int idx=floor(log2(k));
                idx=pow(2,idx);
                k-=idx;
                str1[idx - 1] = ((str1[idx - 1] - '0') ^ (str1[j] - '0')) + '0';
            }
        }
        data_block1.push_back(str1);
    }

    for(int i=0; i<data_block1.size(); i++)
    {
        string tmp=data_block1[i];
        for(int j=0,j1=0; j<tmp.length(); j++)
        {
            if(j==(pow(2,j1)-1))
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout<<tmp[j];
                j1++;
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                cout<<tmp[j];
            }
        }
        cout<<endl;
    }
    //step 3 ended

    //step 4 started
    cout<<endl<<"data bits after column-wise  serialization: "<<endl;
    int len1=data_block1[0].length();
    string serial_data(len1*data_block1.size(),'0');
    int k=0;
    for(int i=0; i<len1; i++)
    {
        for(int j=0; j<data_block1.size(); j++)
            serial_data[k++]=data_block1[j][i];
    }
    cout<<serial_data<<endl;
    //step 4 ended

    //step 5 started
    cout<<endl<<"data bits after appending CRC checksum (sent frame): "<<endl;
    string checksum = compute_crc_checksum(serial_data, g);
    int l1=serial_data.length();
    serial_data+=checksum;
    for(int i=0; i<serial_data.length(); i++)
    {
        if(i>=l1)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
            cout<<serial_data[i];
        }
        else
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout<<serial_data[i];
        }
    }
    cout<<endl;
    //step 5 ended

    //step 6 started
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout<<endl<<"received frame: "<<endl;
    string serial_data1(serial_data.length(),'0');
    for(int i=0; i<serial_data.length(); i++)
    {
        double random = static_cast<double>(rand()) / RAND_MAX;  //generates a random floating-point number between 0.0 (inclusive) and 1.0 (exclusive)
        if(random>=p)
        {
            serial_data1[i]=serial_data[i];
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout<<serial_data1[i];
        }
        else
        {
            serial_data1[i]=(serial_data[i]=='1')?'0':'1';
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            cout<<serial_data1[i];
        }
    }
    cout<<endl;
    //step 6 ended

    //step 7 started
    cout<<endl<<"result of CRC checksum matching: ";
    int ret=verify_received_frame(serial_data1,g);
    if(ret)
        cout<<"No error detected"<<endl;
    else
        cout<<"error detected"<<endl;
    //step 7 ended

    //step 8 started
    cout<<endl<<"data block after removing CRC checksum bits: "<<endl;
    vector<vector<char>> data_block2(row, vector<char>(col, '0')); // it contains the received frame in de-serialized column-major fashion
    vector<vector<char>> data_block3(row, vector<char>(col, '0')); // it marks the position as error or not
    k=0;
    for(int i=0; i<col; i++)
    {
        for(int j=0; j<row; j++)
        {
            data_block2[j][i]=serial_data1[k];
            if(serial_data1[k]==serial_data[k])
                data_block3[j][i]='0';
            else
                data_block3[j][i]='1';
            k++;
        }
    }
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            if(data_block3[i][j]=='0')
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                cout<<data_block2[i][j];
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                cout<<data_block2[i][j];
            }
        }
        cout<<endl;
    }
    //step 8 ended

    //step 9 started
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout<<endl<<"data block after removing check bits:"<<endl;
    vector<vector<char>> data_block4(row, vector<char>(col-r, '0')); // it contain the data block with error after deleting the check bits
    for(int i=0; i<row; i++)
    {
        int k=0;
        for(int j=0,j1=0; j<col; j++)
        {
            if(j==(pow(2,j1)-1))
                j1++;
            else
            {
                data_block4[i][k]=data_block2[i][j];
                k++;
            }
        }
    }
    vector<string> data_block5; // it contain the data block with check bit(recalculated)
    for(int i=0; i<data_block4.size(); i++)
    {
        int len1=col; // total data bit in a row + check bit in a row
        string str1(len1,'0');
        for(int j=0,k=0,j1=0; j<len1; j++)
        {
            if(j==(pow(2,j1)-1))
            {
                str1[j]='0';
                j1++;
            }
            else
            {
                str1[j]=data_block4[i][k];
                k++;
            }
        }
        for(int j=0,j1=0; j<len1; j++)
        {
            if(j==(pow(2,j1)-1))
            {
                j1++;
                continue;
            }
            int k=j+1;
            while(k!=0)
            {
                int idx=floor(log2(k));
                idx=pow(2,idx);
                k-=idx;
                if(idx>0)
                {
                    str1[idx - 1] = ((str1[idx - 1] - '0') ^ (str1[j] - '0')) + '0';
                }
            }
        }
        data_block5.push_back(str1);
    }
    for(int i=0; i<row; i++)
    {
        int count=0;
        for(int j=0,j1=0; j<col; j++)
        {
            if(j==(pow(2,j1)-1))
            {
                if(data_block2[i][j]!=data_block5[i][j])
                {
                    count+=(j+1);
                }
                j1++;
            }
        }
        if(count!=0)
        {
            data_block2[i][count-1]=(data_block2[i][count-1]=='0')?'1':'0';
        }
    }
    vector<string> data_block6; // it will contain data block after correcting error(ignoring check bit)
    for(int i=0; i<row; i++)
    {
        string str1(col-r,'0');
        int jj=0;
        for(int j=0,j1=0; j<col; j++)
        {
            if(j==(pow(2,j1)-1))
                j1++;
            else
            {
                cout<<data_block2[i][j];
                str1[jj++]=data_block2[i][j];
            }
        }
        cout<<endl;
        data_block6.push_back(str1);
    }
    //step 9 ended

    //step 10 started
    cout<<endl<<"output frame: ";
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<(col-r); j+=8)
        {
            string binarySegment = data_block6[i].substr(j, 8);
            bitset<8> binaryValue(binarySegment);
            char asciiChar = static_cast<char>(binaryValue.to_ulong());
            cout<<asciiChar;
        }
    }
    //step 10 ended
    return 0;
}
