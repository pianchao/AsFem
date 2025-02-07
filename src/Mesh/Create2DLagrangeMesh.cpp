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
//+++ Date   : 2020.06.29
//+++ Purpose: Implement the mesh generation for 2D lagrange mesh
//+++          the related mesh type is: quad4,quad8,quad9
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "Mesh/LagrangeMesh.h"

bool LagrangeMesh::Create2DLagrangeMesh(){
    _nNodes=0;_nElmts=0;_nBulkElmts=0;
    _nMinDim=1;_nMaxDim=2;
    _nPhysicalGroups=0;
    _NodeCoords.clear();
    _ElmtConn.clear();
    _PhysicalGroupNameList.clear();
    _PhysicalGroupIDList.clear();
    _PhysicalGroupDimList.clear();

    _PhysicalGroupID2NameList.clear();
    _PhysicalGroupName2IDList.clear();
    _PhysicalGroupName2DimList.clear();
    _PhysicalGroupName2NodesNumPerElmtList.clear();
    _PhysicalName2ElmtIDsList.clear();
    _ElmtVTKCellTypeList.clear();

    //*** for node set physical groups
    _nNodeSetPhysicalGroups=0;
    _NodeSetPhysicalGroupNameList.clear();
    _NodeSetPhysicalGroupIDList.clear();
    _NodeSetPhysicalGroupID2NameList.clear();
    _NodeSetPhysicalGroupName2IDList.clear();
    _NodeSetPhysicalName2NodeIDsList.clear();

    _nNodesPerBulkElmt=0;_nNodesPerSurfaceElmt=0;_nNodesPerLineElmt=0;

    vector<int> tempconn;

    _IsMeshCreated=false;
    //****************************************
    //*** Start to generate mesh
    //****************************************
    double dx,dy;
    int e,i,j,k;
    int i1,i2,i3,i4,i5,i6,i7,i8,i9;
    int nNodesPerBCElmt;

    nNodesPerBCElmt=2;

    if(_BulkMeshType==MeshType::QUAD4){
        _nOrder=1;
        dx=(_Xmax-_Xmin)/(_Nx);
        dy=(_Ymax-_Ymin)/(_Ny);
        _nBulkElmts=_Nx*_Ny;
        _nElmts=_nBulkElmts+2*(_Nx+_Ny);
        _nSurfaceElmts=0;
        _nLineElmts=2*(_Nx+_Ny);
        _nNodes=(_Nx+1)*(_Ny+1);
        _nNodesPerBulkElmt=4;
        _nNodesPerLineElmt=2;
        nNodesPerBCElmt=2;


        _BulkMeshType=MeshType::QUAD4;
        _LineMeshType=MeshType::EDGE2;

        _nNodesPerBulkElmt=4;
        _nNodesPerLineElmt=2;

        _BulkElmtVTKCellType=9;

        _NodeCoords.resize(3*_nNodes,0.0);
        for(j=1;j<=_Ny+1;++j){
            for(i=1;i<=_Nx+1;++i){
                k=(j-1)*(_Nx+1)+i;
                _NodeCoords[(k-1)*3+1-1]=_Xmin+(i-1)*dx;
                _NodeCoords[(k-1)*3+2-1]=_Ymin+(j-1)*dy;
                _NodeCoords[(k-1)*3+3-1]=0.0;
            }
        }
        // Connectivity information for bulk element
        _ElmtConn.resize(_nElmts,vector<int>(0));
        _ElmtVTKCellTypeList.resize(_nElmts,0);
        _ElmtVolume.resize(_nElmts,0.0);
        tempconn.clear();
        for(j=1;j<=_Ny;++j){
            for(i=1;i<=_Nx;++i){
                e=(j-1)*_Nx+i;
                i1=(j-1)*(_Nx+1)+i;
                i2=i1+1;
                i3=i2+_Nx+1;
                i4=i3-1;

                _ElmtConn[e-1+_nElmts-_nBulkElmts].clear();
                _ElmtConn[e-1+_nElmts-_nBulkElmts].push_back(_nNodesPerBulkElmt);
                _ElmtConn[e-1+_nElmts-_nBulkElmts].push_back(i1);
                _ElmtConn[e-1+_nElmts-_nBulkElmts].push_back(i2);
                _ElmtConn[e-1+_nElmts-_nBulkElmts].push_back(i3);
                _ElmtConn[e-1+_nElmts-_nBulkElmts].push_back(i4);
                _ElmtVTKCellTypeList[e-1+_nElmts-_nBulkElmts]=_BulkElmtVTKCellType;

                _ElmtVolume[e-1+_nElmts-_nBulkElmts]=dx*dy;

                tempconn.push_back(e+_nElmts-_nBulkElmts);
            }
        }
    }
    else if(_BulkMeshType==MeshType::QUAD8){
        _nOrder=2;
        dx=(_Xmax-_Xmin)/(2.0*_Nx);
        dy=(_Ymax-_Ymin)/(2.0*_Ny);

        _nBulkElmts=_Nx*_Ny;
        _nElmts=_nBulkElmts+2*(_Nx+_Ny);
        _nLineElmts=2*(_Nx+_Ny);
        _nSurfaceElmts=0;
        _nNodes=(2*_Nx+1)*(2*_Ny+1)-_nBulkElmts;
        _nNodesPerBulkElmt=8;
        _nNodesPerLineElmt=2;
        nNodesPerBCElmt=3;


        _BulkMeshType=MeshType::QUAD8;
        _LineMeshType=MeshType::EDGE3;

        _nNodesPerBulkElmt=8;
        _nNodesPerLineElmt=3;

        _BulkElmtVTKCellType=23;


        // Create node
        _NodeCoords.resize(_nNodes*3,0.0);
        for(j=1;j<=_Ny;++j){
            // for bottom line of each element
            for(i=1;i<=2*_Nx+1;i++){
                k=(j-1)*(2*_Nx+1+_Nx+1)+i;
                _NodeCoords[(k-1)*3+1-1]=_Xmin+(i-1)*dx;
                _NodeCoords[(k-1)*3+2-1]=_Ymin+(j-1)*2*dy;
                _NodeCoords[(k-1)*3+3-1]=0.0;
            }
            // for middle line of each element
            for(i=1;i<=_Nx+1;i++){
                k=(j-1)*(2*_Nx+1+_Nx+1)+2*_Nx+1+i;
                _NodeCoords[(k-1)*3+1-1]=_Xmin+(i-1)*2*dx;
                _NodeCoords[(k-1)*3+2-1]=_Ymin+(j-1)*2*dy+dy;
                _NodeCoords[(k-1)*3+3-1]=0.0;
            }
        }
        // for the last top line
        j=_Ny+1;
        for(i=1;i<=2*_Nx+1;i++){
            k=(j-1)*(2*_Nx+1+_Nx+1)+i;
            _NodeCoords[(k-1)*3+1-1]=_Xmin+(i-1)*dx;
            _NodeCoords[(k-1)*3+2-1]=_Ymin+(j-1)*2*dy;
            _NodeCoords[(k-1)*3+3-1]=0.0;
        }
        // Create connectivity matrix
        _ElmtConn.resize(_nElmts,vector<int>(0));
        _ElmtVTKCellTypeList.resize(_nElmts,0);
        _ElmtVolume.resize(_nElmts,0.0);
        tempconn.clear();
        for(j=1;j<=_Ny;j++){
            for(i=1;i<=_Nx;i++){
                e=(j-1)*_Nx+i;
                i1=(j-1)*(2*_Nx+1+_Nx+1)+2*i-1;
                i2=i1+2;
                i3=i2+(2*_Nx+1+_Nx+1);
                i4=i3-2;

                i5=i1+1;
                i6=i2+(2*_Nx+1)-i;
                i7=i3-1;
                i8=i1+(2*_Nx+1)-(i-1);


                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].clear();
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(_nNodesPerBulkElmt);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i1);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i2);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i3);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i4);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i5);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i6);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i7);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i8);

                _ElmtVTKCellTypeList[e-1+_nElmts-_nBulkElmts]=_BulkElmtVTKCellType;

                tempconn.push_back(e+_nElmts-_nBulkElmts);

                _ElmtVolume[e-1+_nElmts-_nBulkElmts]=2*dx*2*dy;
            }
        }
    }
    else if(_BulkMeshType==MeshType::QUAD9){
        _nOrder=2;
        dx=(_Xmax-_Xmin)/(2.0*_Nx);
        dy=(_Ymax-_Ymin)/(2.0*_Ny);

        _nBulkElmts=_Nx*_Ny;
        _nElmts=_nBulkElmts+2*(_Nx+_Ny);
        _nLineElmts=2*(_Nx+_Ny);
        _nSurfaceElmts=0;
        _nNodes=(2*_Nx+1)*(2*_Ny+1);
        _nNodesPerBulkElmt=9;
        nNodesPerBCElmt=3;

        _BulkMeshType=MeshType::QUAD9;
        _LineMeshType=MeshType::EDGE3;

        _nNodesPerBulkElmt=9;
        _nNodesPerLineElmt=3;

        _BulkElmtVTKCellType=28;

        _NodeCoords.resize(_nNodes*3,0.0);
        for(j=1;j<=2*_Ny+1;j++){
            for(i=1;i<=2*_Nx+1;i++){
                k=(j-1)*(2*_Nx+1)+i;
                _NodeCoords[(k-1)*3+1-1]=_Xmin+(i-1)*dx;
                _NodeCoords[(k-1)*3+2-1]=_Ymin+(j-1)*dy;
                _NodeCoords[(k-1)*3+3-1]=0.0;
            }
        }
        // Create Connectivity matrix
        _ElmtConn.resize(_nElmts,vector<int>(0));
        _ElmtVTKCellTypeList.resize(_nElmts,0);
        _ElmtVolume.resize(_nElmts,0.0);
        tempconn.clear();
        for(j=1;j<=_Ny;j++){
            for(i=1;i<=_Nx;i++){
                e=(j-1)*_Nx+i;
                i1=(j-1)*2*(2*_Nx+1)+2*i-1;
                i2=i1+2;
                i3=i2+2*(2*_Nx+1);
                i4=i3-2;

                i5=i1+1;
                i6=i2+(2*_Nx+1);
                i7=i3-1;
                i8=i1+(2*_Nx+1);
                i9=i8+1;


                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].clear();
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(_nNodesPerBulkElmt);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i1);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i2);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i3);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i4);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i5);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i6);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i7);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i8);
                _ElmtConn[(e-1)+_nElmts-_nBulkElmts].push_back(i9);

                _ElmtVTKCellTypeList[e-1+_nElmts-_nBulkElmts]=_BulkElmtVTKCellType;

                _ElmtVolume[e-1+_nElmts-_nBulkElmts]=2*dx*2*dy;

                tempconn.push_back(e+_nElmts-_nBulkElmts);
            }
        }
    }
    //********************************************************+
    // split the boundary element and bulk element, generate the physical group information
    //********************************************************
    _nPhysicalGroups=4+1;
    _PhysicalGroupNameList.clear();
    _PhysicalGroupNameList.push_back("left");
    _PhysicalGroupNameList.push_back("right");
    _PhysicalGroupNameList.push_back("bottom");
    _PhysicalGroupNameList.push_back("top");
    _PhysicalGroupNameList.push_back("alldomain");

    _PhysicalGroupDimList.clear();
    _PhysicalGroupDimList.push_back(1);
    _PhysicalGroupDimList.push_back(1);
    _PhysicalGroupDimList.push_back(1);
    _PhysicalGroupDimList.push_back(1);
    _PhysicalGroupDimList.push_back(2);

    _PhysicalGroupIDList.clear();
    _PhysicalGroupIDList.push_back(1);
    _PhysicalGroupIDList.push_back(2);
    _PhysicalGroupIDList.push_back(3);
    _PhysicalGroupIDList.push_back(4);
    _PhysicalGroupIDList.push_back(5);

    //******************************
    _PhysicalGroupName2IDList.clear();
    _PhysicalGroupName2IDList.push_back(make_pair("left",     1));
    _PhysicalGroupName2IDList.push_back(make_pair("right",    2));
    _PhysicalGroupName2IDList.push_back(make_pair("bottom",   3));
    _PhysicalGroupName2IDList.push_back(make_pair("top",      4));
    _PhysicalGroupName2IDList.push_back(make_pair("alldomain",5));

    _PhysicalGroupID2NameList.clear();
    _PhysicalGroupID2NameList.push_back(make_pair(1,"left"));
    _PhysicalGroupID2NameList.push_back(make_pair(2,"right"));
    _PhysicalGroupID2NameList.push_back(make_pair(3,"bottom"));
    _PhysicalGroupID2NameList.push_back(make_pair(4,"top"));
    _PhysicalGroupID2NameList.push_back(make_pair(5,"alldomain"));

    // now re-loop all the boundary element to store them into different bc vectors
    vector<int> left,right,bottom,top;
    vector<int> leftnodeids,rightnodeids,bottomnodeids,topnodeids,allnodeids;


    int nBCElmts=0;
    // for leftside
    left.clear();
    leftnodeids.clear();
    i=1;
    for(j=1;j<=_Ny;j++){
        e=(j-1)*_Nx+i;
        _ElmtConn[nBCElmts].clear();
        if(nNodesPerBCElmt==2){
            // quad4 case
            _ElmtConn[nBCElmts].push_back(2);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
            _ElmtVTKCellTypeList[nBCElmts]=3;
            leftnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
            leftnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
        }
        else{
            _ElmtConn[nBCElmts].push_back(3);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,8));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
            _ElmtVTKCellTypeList[nBCElmts]=4;

            leftnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
            leftnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,8));
            leftnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
        }
        nBCElmts+=1;
        left.push_back(nBCElmts);
    }
    // remove the duplicate elements in leftnodeids
    sort(leftnodeids.begin(),leftnodeids.end());
    leftnodeids.erase( unique(leftnodeids.begin(),leftnodeids.end()), leftnodeids.end());
    
    // For right side
    right.clear();
    rightnodeids.clear();
    i=_Nx;
    for(j=1;j<=_Ny;j++){
        _ElmtConn[nBCElmts].clear();
        e=(j-1)*_Nx+i;
        if(nNodesPerBCElmt==2){
            // quad4 case
            _ElmtConn[nBCElmts].push_back(2);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
            _ElmtVTKCellTypeList[nBCElmts]=3;
            rightnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
            rightnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
        }
        else{
            _ElmtConn[nBCElmts].push_back(3);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,6));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
            _ElmtVTKCellTypeList[nBCElmts]=4;
            rightnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
            rightnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,6));
            rightnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
        }
        nBCElmts+=1;
        right.push_back(nBCElmts);
    }
    sort(rightnodeids.begin(),rightnodeids.end());
    rightnodeids.erase( unique(rightnodeids.begin(),rightnodeids.end()), rightnodeids.end());

    // For bottom edge
    bottom.clear();
    bottomnodeids.clear();
    j=1;
    for(i=1;i<=_Nx;i++){
        e=(j-1)*_Nx+i;
        _ElmtConn[nBCElmts].clear();
        if(nNodesPerBCElmt==2){
            // quad4 case
            _ElmtConn[nBCElmts].push_back(2);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
            _ElmtVTKCellTypeList[nBCElmts]=3;
            bottomnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
            bottomnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
        }
        else{
            _ElmtConn[nBCElmts].push_back(3);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,5));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
            _ElmtVTKCellTypeList[nBCElmts]=4;
            bottomnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,1));
            bottomnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,5));
            bottomnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,2));
        }
        nBCElmts+=1;
        bottom.push_back(nBCElmts);
    }
    sort(bottomnodeids.begin(),bottomnodeids.end());
    bottomnodeids.erase( unique(bottomnodeids.begin(),bottomnodeids.end()), bottomnodeids.end());

    // For top edge
    top.clear();
    topnodeids.clear();
    j=_Ny;
    for(i=1;i<=_Nx;i++){
        e=(j-1)*_Nx+i;
        _ElmtConn[nBCElmts].clear();
        if(nNodesPerBCElmt==2){
            // quad4 case
            _ElmtConn[nBCElmts].push_back(2);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
            _ElmtVTKCellTypeList[nBCElmts]=3;
            topnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
            topnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
        }
        else{
            _ElmtConn[nBCElmts].push_back(3);
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,7));
            _ElmtConn[nBCElmts].push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
            _ElmtVTKCellTypeList[nBCElmts]=4;
            topnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,3));
            topnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,7));
            topnodeids.push_back(GetBulkMeshIthBulkElmtJthNodeID(e,4));
        }
        nBCElmts+=1;
        top.push_back(nBCElmts);
    }
    sort(topnodeids.begin(),topnodeids.end());
    topnodeids.erase( unique(topnodeids.begin(),topnodeids.end()), topnodeids.end());
    // for all the nodeids
    allnodeids.resize(_nNodes,0);
    iota(allnodeids.begin(),allnodeids.end(),1);
    
    // add elemental set
    _PhysicalName2ElmtIDsList.clear();
    _PhysicalName2ElmtIDsList.push_back(make_pair("left",left));
    _PhysicalName2ElmtIDsList.push_back(make_pair("right",right));
    _PhysicalName2ElmtIDsList.push_back(make_pair("bottom",bottom));
    _PhysicalName2ElmtIDsList.push_back(make_pair("top",top));
    _PhysicalName2ElmtIDsList.push_back(make_pair("alldomain",tempconn));

    // add nodal sets
    _nNodeSetPhysicalGroups=5;
    _NodeSetPhysicalGroupNameList.clear();
    _NodeSetPhysicalGroupNameList.push_back("left");
    _NodeSetPhysicalGroupNameList.push_back("right");
    _NodeSetPhysicalGroupNameList.push_back("bottom");
    _NodeSetPhysicalGroupNameList.push_back("top");
    _NodeSetPhysicalGroupNameList.push_back("alldomain");
    //***
    _NodeSetPhysicalGroupIDList.clear();
    _NodeSetPhysicalGroupIDList.push_back(1);
    _NodeSetPhysicalGroupIDList.push_back(2);
    _NodeSetPhysicalGroupIDList.push_back(3);
    _NodeSetPhysicalGroupIDList.push_back(4);
    _NodeSetPhysicalGroupIDList.push_back(5);
    //***
    _NodeSetPhysicalGroupID2NameList.clear();
    _NodeSetPhysicalGroupID2NameList.push_back(make_pair(1,"left"));
    _NodeSetPhysicalGroupID2NameList.push_back(make_pair(2,"right"));
    _NodeSetPhysicalGroupID2NameList.push_back(make_pair(3,"bottom"));
    _NodeSetPhysicalGroupID2NameList.push_back(make_pair(4,"top"));
    _NodeSetPhysicalGroupID2NameList.push_back(make_pair(5,"alldomain"));
    //***
    _NodeSetPhysicalGroupName2IDList.clear();
    _NodeSetPhysicalGroupName2IDList.push_back(make_pair("left",1));
    _NodeSetPhysicalGroupName2IDList.push_back(make_pair("right",2));
    _NodeSetPhysicalGroupName2IDList.push_back(make_pair("bottom",3));
    _NodeSetPhysicalGroupName2IDList.push_back(make_pair("top",4));
    _NodeSetPhysicalGroupName2IDList.push_back(make_pair("alldomain",5));
    //***
    _NodeSetPhysicalName2NodeIDsList.clear();
    _NodeSetPhysicalName2NodeIDsList.push_back(make_pair("left",leftnodeids));
    _NodeSetPhysicalName2NodeIDsList.push_back(make_pair("right",rightnodeids));
    _NodeSetPhysicalName2NodeIDsList.push_back(make_pair("bottom",bottomnodeids));
    _NodeSetPhysicalName2NodeIDsList.push_back(make_pair("top",topnodeids));
    _NodeSetPhysicalName2NodeIDsList.push_back(make_pair("alldomain",allnodeids));

    //*********************************
    _PhysicalGroupName2DimList.clear();
    _PhysicalGroupName2DimList.push_back(make_pair("left",     1));
    _PhysicalGroupName2DimList.push_back(make_pair("right",    1));
    _PhysicalGroupName2DimList.push_back(make_pair("bottom",   1));
    _PhysicalGroupName2DimList.push_back(make_pair("top",      1));
    _PhysicalGroupName2DimList.push_back(make_pair("alldomain",2));

    _PhysicalGroupName2NodesNumPerElmtList.clear();
    _PhysicalGroupName2NodesNumPerElmtList.push_back(make_pair("left",_nNodesPerLineElmt));
    _PhysicalGroupName2NodesNumPerElmtList.push_back(make_pair("right",_nNodesPerLineElmt));
    _PhysicalGroupName2NodesNumPerElmtList.push_back(make_pair("bottom",_nNodesPerLineElmt));
    _PhysicalGroupName2NodesNumPerElmtList.push_back(make_pair("top",_nNodesPerLineElmt));
    _PhysicalGroupName2NodesNumPerElmtList.push_back(make_pair("alldomain",_nNodesPerBulkElmt));

    _IsMeshCreated=true;
    return _IsMeshCreated;
}