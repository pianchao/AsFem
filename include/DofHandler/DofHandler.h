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
//+++ Date   : 2020.07.01
//+++ Purpose: Implement general dofhandler for AsFem
//+++          This class can handle both the bulk and interface DoFs
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma once

#include "DofHandler/BulkDofHandler.h"



class DofHandler:public BulkDofHandler{
public:
    DofHandler();

    void PrintAllDofInfo()const{PrintBulkDofInfo();}
    void PrintInterfaceDofInfo()const;
};