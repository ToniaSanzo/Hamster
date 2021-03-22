/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UVector3.h
*/
#pragma once




struct UVector3
{
    // 3D Vector - member variables
    float x = 0.f, y = 0.f, z = 0.f;

    //  UVector3 constructor
    UVector3(float aX = 0.f, float aY = 0.f, float aZ = 0.f) : x(aX), y(aY), z(aZ) {}
};