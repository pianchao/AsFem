//****************************************************************
//* This file is part of the AsFem framework
//* A Simple Finite Element Method program (AsFem)
//* All rights reserved, Yang Bai/M3 Group @ CopyRight 2022
//* https://github.com/M3Group/AsFem
//* Licensed under GNU GPLv3, please see LICENSE for details
//* https://www.gnu.org/licenses/gpl-3.0.en.html
//****************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ Author : Yang Bai
//+++ Date   : 2020.06.26
//+++ Purpose: implement the mesh import for msh(ver=2) file format
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "Mesh/Gmsh2IO.h"

bool Gmsh2IO::ReadMeshFromFile(Mesh &mesh){
    char longbuff[105],buff[55];
    string str,substr;
    if(!_HasSetMeshFileName){
        MessagePrinter::PrintErrorTxt("can\'t read mesh, the mesh file name has not been set");
        return false;
    }
    _in.open(_MeshFileName.c_str(),ios::in);
    if(!_in.is_open()){
        snprintf(longbuff,105,"can\'t read the .msh file(=%25s)      ,please make sure file name is correct",_MeshFileName.c_str());
        str=longbuff;
        MessagePrinter::PrintErrorTxt(str);
        MessagePrinter::AsFem_Exit();
    }
    double version;
    int format,size;

    //*** initialize
    mesh.GetBulkMeshNodeCoordsPtr().clear();
    mesh.GetBulkMeshElmtConnPtr().clear();
    mesh.GetBulkMeshElmtVolumePtr().clear();

    mesh.GetBulkMeshElmtVTKCellTypeListPtr().clear();
    mesh.GetBulkMeshElmtPhyIDListPtr().clear();
    mesh.GetBulkMeshElmtDimListPtr().clear();
    mesh.GetBulkMeshElmtMeshTypeListPtr().clear();
    //*** for physical groups
    mesh.GetBulkMeshPhysicalGroupNameListPtr().clear();
    mesh.GetBulkMeshPhysicalGroupIDListPtr().clear();
    mesh.GetBulkMeshPhysicalGroupDimListPtr().clear();
    mesh.GetBulkMeshPhysicalGroupID2NameListPtr().clear();
    mesh.GetBulkMeshPhysicalGroupName2IDListPtr().clear();
    mesh.GetBulkMeshPhysicalGroupName2NodesNumPerElmtListPtr().clear();
    mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().clear();
    //*** for nodeset physical group information
    mesh.GetBulkMeshNodeSetPhysicalNameListPtr().clear();
    mesh.GetBulkMeshNodeSetPhysicalIDListPtr().clear();
    mesh.GetBulkMeshNodeSetPhysicalID2NameListPtr().clear();
    mesh.GetBulkMeshNodeSetPhysicalName2IDListPtr().clear();
    mesh.GetBulkMeshNodeSetPhysicalName2NodeIDsListPtr().clear();

    mesh.SetBulkMeshDim(0);
    mesh.SetBulkMeshNodesNum(0);
    mesh.SetBulkMeshElmtsNum(0);
    mesh.SetBulkMeshBulkElmtsNum(0);
    mesh.SetBulkMeshSurfaceElmtsNum(0);
    mesh.SetBulkMeshLineElmtsNum(0);
    mesh.SetBulkMeshPhysicalGroupNums(0);
    mesh.SetBulkMeshNodeSetPhysicalGroupNums(0);

    _nMaxDim=-1;_nMinDim=4;
    _nPhysicGroups=0;
    _nNodeSetPhysicalGroups=0;

    vector<pair<int,int>> UniquePhyDim2IDList;
    int MaxPhyIDofPhyGroup=-1,MaxPhyIDofElmt=-1;

    map<int,vector<int>> NodeSetPhyID2NodeIDsList;


    _Xmax=-1.0e16;_Xmin=1.0e16;
    _Ymax=-1.0e16;_Ymin=1.0e16;
    _Ymax=-1.0e16;_Ymin=1.0e16;


    while(!_in.eof()){
        // now we start to read *.msh file
        getline(_in,str);
        if(str.find("$MeshFormat")!=string::npos){
            _in>>version>>format>>size;
            if((version!=2.0)&&(version!=2.1)&&(version!=2.2)){
                // currently, only the gmsh2 format is supported!!!
                snprintf(buff,50,"version=%12.3f is not supported yet",version);
                str=buff;
                MessagePrinter::PrintErrorTxt(str);
                return false;
            }
        }// end-of-versioin-read
        else if(str.find("$PhysicalNames")!=string::npos){
            _nPhysicGroups=0;
            mesh.GetBulkMeshPhysicalGroupNameListPtr().clear();
            mesh.GetBulkMeshPhysicalGroupIDListPtr().clear();
            mesh.GetBulkMeshPhysicalGroupDimListPtr().clear();
            mesh.GetBulkMeshPhysicalGroupID2NameListPtr().clear();
            mesh.GetBulkMeshPhysicalGroupName2IDListPtr().clear();
            mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().clear();
            mesh.GetBulkMeshPhysicalGroupName2NodesNumPerElmtListPtr().clear();
            mesh.SetBulkMeshPhysicalGroupNums(0);
            //******************************************************
            //*** node-set physical information
            mesh.GetBulkMeshNodeSetPhysicalNameListPtr().clear();
            mesh.GetBulkMeshNodeSetPhysicalIDListPtr().clear();
            mesh.GetBulkMeshNodeSetPhysicalID2NameListPtr().clear();
            mesh.GetBulkMeshNodeSetPhysicalName2IDListPtr().clear();
            mesh.GetBulkMeshNodeSetPhysicalName2NodeIDsListPtr().clear();
            mesh.SetBulkMeshNodeSetPhysicalGroupNums(0);

            int phydim,phyid;
            string phyname;
            _in>>_nPhysicGroups;
            getline(_in,str);//remove \n in this line
            for(int i=0;i<_nPhysicGroups;i++){
                getline(_in,str);
                istringstream s_stream(str);
                s_stream>>phydim>>phyid>>phyname;
                // remove the '"' ,keep only the text
                phyname.erase(std::remove(phyname.begin(),phyname.end(),'"'),phyname.end());
                mesh.GetBulkMeshPhysicalGroupNameListPtr().push_back(phyname);
                mesh.GetBulkMeshPhysicalGroupIDListPtr().push_back(phyid);
                mesh.GetBulkMeshPhysicalGroupDimListPtr().push_back(phydim);

                mesh.GetBulkMeshPhysicalGroupID2NameListPtr().push_back(make_pair(phyid,phyname));
                mesh.GetBulkMeshPhysicalGroupName2IDListPtr().push_back(make_pair(phyname,phyid));


                if(phydim>_nMaxDim) _nMaxDim=phydim;
                if(_nMinDim<phydim) _nMinDim=phydim;
                if(phyid>MaxPhyIDofPhyGroup) MaxPhyIDofPhyGroup=phyid;
                if(phydim==0){
                    // for nodal physical information
                    _nNodeSetPhysicalGroups+=1;
                    mesh.GetBulkMeshNodeSetPhysicalNameListPtr().push_back(phyname);
                    mesh.GetBulkMeshNodeSetPhysicalIDListPtr().push_back(phyid);
                    mesh.GetBulkMeshNodeSetPhysicalID2NameListPtr().push_back(make_pair(phyid,phyname));
                    mesh.GetBulkMeshNodeSetPhysicalName2IDListPtr().push_back(make_pair(phyname,phyid));
                    mesh.SetBulkMeshNodeSetPhysicalGroupNums(_nNodeSetPhysicalGroups);
                }
            }
        }//end-of-physical-group-information
        else if(str.find("$Nodes")!=string::npos){
            // read the nodes' coordinates
            // node-id, x, y, z
            _nNodes=0;
            _in>>_nNodes;
            mesh.SetBulkMeshNodesNum(_nNodes);
            mesh.GetBulkMeshNodeCoordsPtr().resize(_nNodes*3,0.0);
            int id;
            double x,y,z;
            _Xmax=-1.0e16;_Xmin=1.0e16;
            _Ymax=_Xmax;_Ymin=_Xmin;
            _Zmax=_Xmax;_Zmin=_Xmin;
            for(int i=0;i<_nNodes;i++){
                _in>>id>>x>>y>>z;
                mesh.GetBulkMeshNodeCoordsPtr()[(id-1)*3+1-1]=x;
                mesh.GetBulkMeshNodeCoordsPtr()[(id-1)*3+2-1]=y;
                mesh.GetBulkMeshNodeCoordsPtr()[(id-1)*3+3-1]=z;

                if(x>_Xmax) _Xmax=x;
                if(x<_Xmin) _Xmin=x;
                if(y>_Ymax) _Ymax=y;
                if(y<_Ymin) _Ymin=y;
                if(z>_Zmax) _Zmax=z;
                if(z<_Zmin) _Zmin=z;
            }
            getline(_in,str);
        }//end-of-node-reading
        else if(str.find("$Elements")!=string::npos){
            // here the element constains all the element(no matter it is line elmt or bulk elmt)
            _nElmts=0;
            _in>>_nElmts;
            mesh.SetBulkMeshElmtsNum(_nElmts);
            mesh.GetBulkMeshElmtVolumePtr().resize(_nElmts,0.0);
            mesh.GetBulkMeshElmtDimListPtr().resize(_nElmts,0);
            mesh.GetBulkMeshElmtConnPtr().resize(_nElmts,vector<int>(0));
            mesh.GetBulkMeshElmtVTKCellTypeListPtr().resize(_nElmts,0);
            mesh.GetBulkMeshElmtPhyIDListPtr().resize(_nElmts,0);
            mesh.GetBulkMeshElmtMeshTypeListPtr().resize(_nElmts,MeshType::NULLTYPE);

            int elmtid,phyid,geoid,ntags,elmttype,vtktype;
            int nodes,dim,maxdim,elmtorder;
            vector<int> tempconn;
            MeshType meshtype,bcmeshtype;
            string meshtypename;

            _nNodesPerBulkElmt=-1;
            _nNodesPerLineElmt=0;
            _nNodesPerSurfaceElmt=0;
            maxdim=-1;
            _nOrder=1;

            bool IsUnique;

            int nSurfaceElmts,nLineElmts,nBulkElmts;
            nSurfaceElmts=0;nLineElmts=0;nBulkElmts=0;

            NodeSetPhyID2NodeIDsList.clear();

            for(int e=0;e<_nElmts;e++){
                _in>>elmtid>>elmttype>>ntags>>phyid>>geoid;

                if(phyid==0) phyid=geoid;

                nodes=GetElmtNodesNumFromGmshElmtType(elmttype);
                dim=GetElmtDimFromGmshElmtType(elmttype);
                vtktype=GetElmtVTKCellTypeFromGmshElmtType(elmttype);
                meshtype=GetElmtMeshTypeFromGmshElmtType(elmttype);
                meshtypename=GetElmtTypeNameFromGmshElmtType(elmttype);
                bcmeshtype=GetSubElmtMeshTypeFromGmshElmtType(elmttype);
                elmtorder=GetElmtOrderFromGmshElmtType(elmttype);
                if(elmtorder>_nOrder) _nOrder=elmtorder;



                if(dim==1){
                    _nNodesPerLineElmt=nodes;
                    _nNodesPerBulkElmt=nodes;
                    mesh.SetBulkMeshMeshTypeName(meshtypename);
                    mesh.SetBulkMeshMeshType(meshtype);
                    mesh.SetBulkMeshLineMeshType(meshtype);
                    nLineElmts+=1;
                }
                if(dim==2){
                    _nNodesPerSurfaceElmt=nodes;
                    _nNodesPerBulkElmt=nodes;
                    mesh.SetBulkMeshMeshType(meshtype);
                    mesh.SetBulkMeshMeshTypeName(meshtypename);
                    mesh.SetBulkMeshLineMeshType(bcmeshtype);
                    nSurfaceElmts+=1;
                }
                if(dim==3){
                    _nNodesPerBulkElmt=nodes;
                    mesh.SetBulkMeshMeshType(meshtype);
                    mesh.SetBulkMeshMeshTypeName(meshtypename);
                    mesh.SetBulkMeshSurfaceMeshType(bcmeshtype);
                    nBulkElmts+=1;
                }
                
                if(dim>maxdim) maxdim=dim;
                if(dim>_nMaxDim) _nMaxDim=dim;
                if(dim<_nMinDim) _nMinDim=dim;
                if(phyid>MaxPhyIDofElmt) MaxPhyIDofElmt=phyid;

                mesh.GetBulkMeshElmtConnPtr()[elmtid-1].resize(nodes+1,0);
                mesh.GetBulkMeshElmtConnPtr()[elmtid-1][0]=nodes;

                tempconn.resize(nodes,0);
                for(int j=0;j<nodes;j++){
                    _in>>tempconn[j];
                }

                if(dim==0){
                    // for node set
                    if(tempconn.size()!=1){
                        MessagePrinter::PrintErrorTxt("invalid node set(element) in your msh2 file, the connectivity array should only contain 1 element(node id)");
                        MessagePrinter::AsFem_Exit();
                    }
                    NodeSetPhyID2NodeIDsList[phyid].push_back(tempconn[0]);
                }

                for(int j=0;j<nodes;j++){
                    mesh.GetBulkMeshElmtConnPtr()[elmtid-1][j+1]=tempconn[j];
                }

                mesh.GetBulkMeshElmtDimListPtr()[elmtid-1]=dim;
                mesh.GetBulkMeshElmtMeshTypeListPtr()[elmtid-1]=meshtype;
                mesh.GetBulkMeshElmtPhyIDListPtr()[elmtid-1]=phyid;
                mesh.GetBulkMeshElmtVTKCellTypeListPtr()[elmtid-1]=vtktype;

                if(UniquePhyDim2IDList.size()==0){
                    UniquePhyDim2IDList.push_back(make_pair(dim,phyid));
                }
                else{
                    IsUnique=true;
                    for(const auto &it:UniquePhyDim2IDList){
                        if(it.first==dim && it.second==phyid){
                            IsUnique=false;
                            break;
                        }
                    }
                    if(IsUnique){
                        UniquePhyDim2IDList.push_back(make_pair(dim,phyid));
                    }
                }

            }//--->end-of-element-loop
            mesh.SetBulkMeshMeshOrder(_nOrder);
            mesh.SetBulkMeshMinDim(_nMinDim);
            mesh.SetBulkMeshMaxDim(_nMaxDim);
            mesh.SetBulkMeshNodesNumPerBulkElmt(_nNodesPerBulkElmt);
            mesh.SetBulkMeshNodesNumPerSurfaceElmt(_nNodesPerSurfaceElmt);
            mesh.SetBulkMeshNodesNumPerLineElmt(_nNodesPerLineElmt);
            mesh.SetBulkMeshLineElmtsNum(nLineElmts);
            mesh.SetBulkMeshSurfaceElmtsNum(nSurfaceElmts);
            if(_nMaxDim==1){
                mesh.SetBulkMeshBulkElmtsNum(nLineElmts);
                _nBulkElmts=nLineElmts;
            }
            else if(_nMaxDim==2){
                mesh.SetBulkMeshBulkElmtsNum(nSurfaceElmts);
                _nBulkElmts=nSurfaceElmts;
            }
            else if(_nMaxDim==3){
                mesh.SetBulkMeshBulkElmtsNum(nBulkElmts);
                _nBulkElmts=nBulkElmts;
            }
            
        }//end-of-read-element-info
        
    }// end-of-while(!_in.eof())


    vector<int> bulkconn;
    vector<vector<int>> phy2conn(UniquePhyDim2IDList.size(),vector<int>(0));

    if(_nPhysicGroups<1){
        // this means, no any physical group information is given in your msh file
        // so, we have to mark all the bulk elements as the "alldomain" physical group
        int maxid=-1;
        int phyid,dim;
        string phyname;
        for(const auto &it:UniquePhyDim2IDList){
            dim=it.first;
            phyid=it.second;
            phyname=to_string(phyid);
            if(phyid>maxid) maxid=phyid;
            if(dim==_nMaxDim){
                mesh.GetBulkMeshPhysicalGroupIDListPtr().push_back(phyid);
                mesh.GetBulkMeshPhysicalGroupDimListPtr().push_back(_nMaxDim);
                mesh.GetBulkMeshPhysicalGroupNameListPtr().push_back(phyname);

                mesh.GetBulkMeshPhysicalGroupID2NameListPtr().push_back(make_pair(phyid,phyname));
                mesh.GetBulkMeshPhysicalGroupName2IDListPtr().push_back(make_pair(phyname,phyid));
            }
        }
        bulkconn.clear();
        for(int e=0;e<_nElmts;e++){
            if(mesh.GetBulkMeshIthElmtDim(e+1)==_nMaxDim){
                bulkconn.push_back(e+1);
                for(int j=0;j<static_cast<int>(phy2conn.size());j++){
                    if(mesh.GetBulkMeshIthElmtPhyID(e+1)==UniquePhyDim2IDList[j].second){
                        phy2conn[j].push_back(e+1);
                        break;
                    }
                }
            }
        }

        for(int j=0;j<static_cast<int>(phy2conn.size());j++){
            phyname=mesh.GetBulkMeshIthPhysicalName(j+1);
            mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().push_back(make_pair(phyname,phy2conn[j]));
        }

        mesh.SetBulkMeshPhysicalGroupNums(1+phy2conn.size());
        mesh.GetBulkMeshPhysicalGroupNameListPtr().push_back("alldomain");
        mesh.GetBulkMeshPhysicalGroupDimListPtr().push_back(_nMaxDim);
        mesh.GetBulkMeshPhysicalGroupIDListPtr().push_back(maxid+1);
        mesh.GetBulkMeshPhysicalGroupID2NameListPtr().push_back(make_pair(maxid+1,"alldomain"));
        mesh.GetBulkMeshPhysicalGroupName2IDListPtr().push_back(make_pair("alldomain",maxid+1));
        mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().push_back(make_pair("alldomain",bulkconn));

    }
    else if(_nPhysicGroups==1&&UniquePhyDim2IDList.size()==1){
        // for the case where only 1 physical group is defined
        bulkconn.clear();
        int maxid=-1;
        for(int e=0;e<_nElmts;e++){
            if(mesh.GetBulkMeshIthElmtDim(e+1)==_nMaxDim){
                bulkconn.push_back(e+1);
                if(mesh.GetBulkMeshIthElmtPhyID(e+1)>maxid) maxid=mesh.GetBulkMeshIthElmtPhyID(e+1);
            }
        }

        mesh.SetBulkMeshPhysicalGroupNums(2);
        mesh.GetBulkMeshPhysicalGroupNameListPtr().push_back("alldomain");
        mesh.GetBulkMeshPhysicalGroupIDListPtr().push_back(maxid+1);
        mesh.GetBulkMeshPhysicalGroupName2IDListPtr().push_back(make_pair("alldomain",maxid+1));
        mesh.GetBulkMeshPhysicalGroupID2NameListPtr().push_back(make_pair(maxid+1,"alldomain"));

        mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().push_back(make_pair(mesh.GetBulkMeshIthPhysicalName(1),bulkconn));
        mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().push_back(make_pair("alldomain",bulkconn));

    }
    else{
        string phyname;
        bulkconn.clear();
        phy2conn.resize(_nPhysicGroups,vector<int>(0));

        for(int e=0;e<_nElmts;e++){
            for(int j=0;j<static_cast<int>(phy2conn.size());j++){
                if(mesh.GetBulkMeshIthElmtPhyID(e+1)==mesh.GetBulkMeshIthPhysicalID(j+1)){
                    phy2conn[j].push_back(e+1);
                    break;
                }
            }
            if(mesh.GetBulkMeshIthElmtDim(e+1)==_nMaxDim){
                bulkconn.push_back(e+1);
            }
        }

        for(int j=0;j<static_cast<int>(phy2conn.size());j++){
            phyname=mesh.GetBulkMeshIthPhysicalName(j+1);
            mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().push_back(make_pair(phyname,phy2conn[j]));
        }

        mesh.SetBulkMeshPhysicalGroupNums(1+phy2conn.size());
        mesh.GetBulkMeshPhysicalGroupNameListPtr().push_back("alldomain");
        mesh.GetBulkMeshPhysicalGroupDimListPtr().push_back(_nMaxDim);
        mesh.GetBulkMeshPhysicalGroupIDListPtr().push_back(MaxPhyIDofElmt+1);
        mesh.GetBulkMeshPhysicalGroupID2NameListPtr().push_back(make_pair(MaxPhyIDofElmt+1,"alldomain"));
        mesh.GetBulkMeshPhysicalGroupName2IDListPtr().push_back(make_pair("alldomain",MaxPhyIDofElmt+1));
        mesh.GetBulkMeshPhysicalName2ElmtIDsListPtr().push_back(make_pair("alldomain",bulkconn));
    }

    mesh.GetBulkMeshPhysicalGroupName2NodesNumPerElmtListPtr().clear();
    int dim;string phyname;
    for(int i=0;i<_nPhysicGroups;i++){
        phyname=mesh.GetBulkMeshPhysicalGroupNameListPtr()[i];
        dim=mesh.GetBulkMeshPhysicalGroupDimListPtr()[i];
        if(dim==1){
            mesh.GetBulkMeshPhysicalGroupName2NodesNumPerElmtListPtr().push_back(make_pair(phyname,_nNodesPerLineElmt));
        }
        else if(dim==2){
            mesh.GetBulkMeshPhysicalGroupName2NodesNumPerElmtListPtr().push_back(make_pair(phyname,_nNodesPerSurfaceElmt));
        }
        else if(dim==3){
            mesh.GetBulkMeshPhysicalGroupName2NodesNumPerElmtListPtr().push_back(make_pair(phyname,_nNodesPerBulkElmt));
        }
    }

    if(_nNodeSetPhysicalGroups>static_cast<int>(NodeSetPhyID2NodeIDsList.size())){
        // the number of node-set physical group should not beyond the number of physical id you give in your $Elements block for all the nodes
        // the node-set physical group number could be greater than the one you defined in your $Physical block !!!
        MessagePrinter::PrintErrorTxt("your node set physical group numbers can not be greater than the element's physical id numbers");
        MessagePrinter::AsFem_Exit();
    }
    int phyid;
    bool HasNodePhyID;
    for(int i=0;i<_nNodeSetPhysicalGroups;i++){
        phyid=mesh.GetBulkMeshNodeSetPhysicalIDListPtr()[i];
        phyname=mesh.GetBulkMeshNodeSetPhysicalNameListPtr()[i];
        HasNodePhyID=false;
        for(auto it:NodeSetPhyID2NodeIDsList){
            if(phyid==it.first){
                mesh.GetBulkMeshNodeSetPhysicalName2NodeIDsListPtr().push_back(make_pair(phyname,it.second));
                HasNodePhyID=true;
                break;
            }
        }
        if(!HasNodePhyID){
            MessagePrinter::PrintErrorTxt("you defined "+phyname+" in your $Physical block of the msh2 file, however, we can not find the related nodes in $Elements block");
            MessagePrinter::AsFem_Exit();
        }
    }

    return _nBulkElmts>0;
}