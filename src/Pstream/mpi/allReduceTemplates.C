/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "allReduce.H"

// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

template<class Type, class BinaryOp>
void Foam::allReduce
(
    Type& Value,
    int MPICount,
    MPI_Datatype MPIType,
    MPI_Op MPIOp,
    const BinaryOp& bop,
    const int tag
)
{
    if (!UPstream::parRun())
    {
        return;
    }

    if (UPstream::nProcs() <= UPstream::nProcsSimpleSum)
    {
        if (UPstream::master())
        {
            for
            (
                int slave=UPstream::firstSlave();
                slave<=UPstream::lastSlave();
                slave++
            )
            {
                Type value;

                if
                (
                    MPI_Recv
                    (
                        &value,
                        MPICount,
                        MPIType,
                        UPstream::procID(slave),
                        tag,
                        MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE
                    )
                )
                {
                    FatalErrorIn
                    (
                        "void Foam::allReduce\n"
                        "(\n"
                        "    Type&,\n"
                        "    int,\n"
                        "    MPI_Datatype,\n"
                        "    MPI_Op,\n"
                        "    const BinaryOp&,\n"
                        "    const int\n"
                        ")\n"
                    )   << "MPI_Recv failed"
                        << Foam::abort(FatalError);
                }

                Value = bop(Value, value);
            }
        }
        else
        {
            if
            (
                MPI_Send
                (
                    &Value,
                    MPICount,
                    MPIType,
                    UPstream::procID(UPstream::masterNo()),
                    tag,
                    MPI_COMM_WORLD
                )
            )
            {
                FatalErrorIn
                (
                    "void Foam::allReduce\n"
                    "(\n"
                    "    Type&,\n"
                    "    int,\n"
                    "    MPI_Datatype,\n"
                    "    MPI_Op,\n"
                    "    const BinaryOp&,\n"
                    "    const int\n"
                    ")\n"
                )   << "MPI_Send failed"
                    << Foam::abort(FatalError);
            }
        }


        if (UPstream::master())
        {
            for
            (
                int slave=UPstream::firstSlave();
                slave<=UPstream::lastSlave();
                slave++
            )
            {
                if
                (
                    MPI_Send
                    (
                        &Value,
                        MPICount,
                        MPIType,
                        UPstream::procID(slave),
                        tag,
                        MPI_COMM_WORLD
                    )
                )
                {
                    FatalErrorIn
                    (
                        "void Foam::allReduce\n"
                        "(\n"
                        "    Type&,\n"
                        "    int,\n"
                        "    MPI_Datatype,\n"
                        "    MPI_Op,\n"
                        "    const BinaryOp&,\n"
                        "    const int\n"
                        ")\n"
                    )   << "MPI_Send failed"
                        << Foam::abort(FatalError);
                }
            }
        }
        else
        {
            if
            (
                MPI_Recv
                (
                    &Value,
                    MPICount,
                    MPIType,
                    UPstream::procID(UPstream::masterNo()),
                    tag,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                )
            )
            {
                FatalErrorIn
                (
                    "void Foam::allReduce\n"
                    "(\n"
                    "    Type&,\n"
                    "    int,\n"
                    "    MPI_Datatype,\n"
                    "    MPI_Op,\n"
                    "    const BinaryOp&,\n"
                    "    const int\n"
                    ")\n"
                )   << "MPI_Recv failed"
                    << Foam::abort(FatalError);
            }
        }
    }
    else
    {
        Type sum;
        MPI_Allreduce(&Value, &sum, MPICount, MPIType, MPIOp, MPI_COMM_WORLD);
        Value = sum;
    }
}


// ************************************************************************* //
