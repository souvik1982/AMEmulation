const unsigned int layersOnChip=8;

int superStripCoordinate(int layer, int ladder, int module, int seg, int strip)
{
  int coord=0;
  
  if (strip>=0 && strip<1024) coord=0; else {std::cout<<"ERROR: Strip address is less than 0 or greater than 1023"<<std::endl; return -1;}
  if (seg>=0 && seg<=31) coord+=(seg<<10); else {std::cout<<"ERROR: Segment is less than 0 or greater than 31; layer = "<<layer<<std::endl; return -1;}
  if (module>=0 && module<80) coord+=(module<<(5+10)); else {std::cout<<"ERROR: Module is less than 0 or greater than 79; layer = "<<layer<<std::endl; return -1;}
  if (ladder>=0 && ladder<100) coord+=(ladder<<(7+5+10)); else {std::cout<<"ERROR: Ladder is less than 0 or greater than 99; layer = "<<layer<<std::endl; return -1;}
  
  return coord;
}

int returnAMChipLayer(int layer)
{
  int ret=12;
  if (layer==5) ret=0;
  else if (layer==6 || layer==15 || layer==22) ret=1;
  else if (layer==7 || layer==14 || layer==21) ret=2;
  else if (layer==8 || layer==13 || layer==20) ret=3;
  else if (layer==12 || layer==19) ret=4;
  else if (layer==9 || layer==11 || layer==18) ret=5;
  else if (layer==10) ret=6;
  else std::cout<<"ERROR: Non layer "<<layer<<" hit."<<std::endl;
  return ret;
}
