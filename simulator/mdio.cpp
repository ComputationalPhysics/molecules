#include <mdio.h>
#include <system.h>
#include <settings.h>
#include <iostream>
#include <QFile>
#include <QDebug>
//#include <mdtimer.h>

MDIO::MDIO()
{

}

void MDIO::setup(System *system_) {
    system = system_;
    settings = system->settings;
    movie_file_open = false;
    energy_file = fopen("statistics/energy.txt","w");
    pressure_file = fopen("statistics/pressure.txt","w");
    velocity_file = fopen("statistics/velocity.txt","w");
    count_periodic_file = fopen("statistics/count_periodic.txt", "w");
}

//void MDIO::save_state_to_movie_file() {
////    system->mdtimer->start_io();
//    if(settings->create_movie && !(system->steps % settings->movie_every_n_frame)) {
//        if(!movie_file_open) {
//            char *filename = new char[100];
//            sprintf(filename,"movie_files/movie%04d.bin",system->myid);
//            movie_file = new ofstream(filename,ios::out | ios::binary);
//            movie_file_open = true;
//            data = new double[3*system->max_number_of_atoms];

//            delete filename;
//        }

//        for(unsigned long n=0;n<system->num_atoms_local;n++) {
//            data[3*n+0] = system->positions[3*n+0] + system->origo[0];
//            data[3*n+1] = system->positions[3*n+1] + system->origo[1];
//            data[3*n+2] = system->positions[3*n+2] + system->origo[2];
//        }

//        movie_file->write (reinterpret_cast<char*>(&system->num_atoms_local), sizeof(unsigned long));
//        movie_file->write (reinterpret_cast<char*>(data), 3*system->num_atoms_local*sizeof(double));
//        movie_file->write (reinterpret_cast<char*>(system->atom_type), system->num_atoms_local*sizeof(unsigned long));
//        movie_file->write (reinterpret_cast<char*>(system->atom_ids), system->num_atoms_local*sizeof(unsigned long));
//        movie_file->flush();
//    }
////    system->mdtimer->end_io();
//}

void MDIO::save_state_to_file_binary(QString fileName) {
    QFile file (fileName);
    file.open(QIODevice::WriteOnly);

    int step = 1;
    int atomCount = system->num_atoms;
    double minimumX = 0.0;
    double minimumY = 0.0;
    double minimumZ = 0.0;
    double maximumX = system->systemSize().x();
    double maximumY = system->systemSize().y();
    double maximumZ = system->systemSize().z();
    double shear = 0.0;
    int columnCount = 1 + 1 + 3 + 3;
    int chunkCount = 1;
    int chunkLength = atomCount * columnCount;
    double *tmp_data = new double[chunkLength];

    file.write(reinterpret_cast<char*>(&step), sizeof(int));
    file.write(reinterpret_cast<char*>(&atomCount), sizeof(int));
    file.write(reinterpret_cast<char*>(&minimumX), sizeof(double));
    file.write(reinterpret_cast<char*>(&maximumX), sizeof(double));
    file.write(reinterpret_cast<char*>(&minimumY), sizeof(double));
    file.write(reinterpret_cast<char*>(&maximumY), sizeof(double));
    file.write(reinterpret_cast<char*>(&minimumZ), sizeof(double));
    file.write(reinterpret_cast<char*>(&maximumZ), sizeof(double));
    file.write(reinterpret_cast<char*>(&shear), sizeof(double));
    file.write(reinterpret_cast<char*>(&shear), sizeof(double));
    file.write(reinterpret_cast<char*>(&shear), sizeof(double));
    file.write(reinterpret_cast<char*>(&columnCount), sizeof(int));
    file.write(reinterpret_cast<char*>(&chunkCount), sizeof(int));
    file.write(reinterpret_cast<char*>(&chunkLength), sizeof(int));

    for(unsigned int i=0;i<system->num_atoms;i++) {
        int offset = 0;
        double index = i + 1;
        double type = system->atom_type[i];
        tmp_data[columnCount*i + offset + 0] = index; // ID
        tmp_data[columnCount*i + offset + 1] = type; // atom type
        offset += 2;

        // Position
        tmp_data[columnCount*i + offset + 0] = system->positions[3*i+0];
        tmp_data[columnCount*i + offset + 1] = system->positions[3*i+1];
        tmp_data[columnCount*i + offset + 2] = system->positions[3*i+2];
        offset += 3;

        // Velocity
        tmp_data[columnCount*i + offset + 0] = system->velocities[3*i+0];
        tmp_data[columnCount*i + offset + 1] = system->velocities[3*i+1];
        tmp_data[columnCount*i + offset + 2] = system->velocities[3*i+2];
        offset += 3;
    }

    file.write (reinterpret_cast<char*>(tmp_data), chunkLength*sizeof(double));

    file.close();
    delete tmp_data;
    //    system->mdtimer->end_io();
}

void MDIO::load_state_from_file_binary(QString fileName) {
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if(!file.isOpen()) {
        qDebug() << "WARNING: Could not open" << fileName;
        return;
    }

    int step = -1;
    int atomCount = -1;
    double minimumX = -1.0;
    double minimumY = -1.0;
    double minimumZ = -1.0;
    double maximumY = -1.0;
    double maximumX = -1.0;
    double maximumZ = -1.0;
    double shear = -1.0;
    int columnCount = -1;
    int chunkCount = -1;
    int chunkLength = -1;

    file.read(reinterpret_cast<char*>(&step), sizeof(int));
    file.read(reinterpret_cast<char*>(&atomCount), sizeof(int));
    file.read(reinterpret_cast<char*>(&minimumX), sizeof(double));
    file.read(reinterpret_cast<char*>(&maximumX), sizeof(double));
    file.read(reinterpret_cast<char*>(&minimumY), sizeof(double));
    file.read(reinterpret_cast<char*>(&maximumY), sizeof(double));
    file.read(reinterpret_cast<char*>(&minimumZ), sizeof(double));
    file.read(reinterpret_cast<char*>(&maximumZ), sizeof(double));
    file.read(reinterpret_cast<char*>(&shear), sizeof(double));
    file.read(reinterpret_cast<char*>(&shear), sizeof(double));
    file.read(reinterpret_cast<char*>(&shear), sizeof(double));
    file.read(reinterpret_cast<char*>(&columnCount), sizeof(int));
    file.read(reinterpret_cast<char*>(&chunkCount), sizeof(int));
    file.read(reinterpret_cast<char*>(&chunkLength), sizeof(int));

    if(columnCount != 1 + 1 + 3 + 3) {
        cout << "Unexpected number of columns. Aborting!" << endl;
        return;
    }

    cout << "step: " << step << " atomcount: " << atomCount << " minimumX: " << minimumX << endl;
    cout << "chunk: " << columnCount << " " << chunkCount << " " << chunkLength << endl;
    cout << "bounds: " << maximumX << " " << maximumY << " " << maximumZ << endl;

    system->num_atoms = atomCount;
    system->setSystemSize(QVector3D(maximumX - minimumX, maximumY - minimumY, maximumZ - minimumZ));

    double *tmp_data = new double[chunkLength];
    file.read(reinterpret_cast<char*>(tmp_data),chunkLength*sizeof(double));
    file.close();

    for(unsigned int i=0;i<system->num_atoms;i++) {
        int offset = 0;
        system->atom_type[i] = tmp_data[columnCount*i + offset + 1];
        offset += 2;

        double positionX = tmp_data[columnCount*i+offset+0];
        double positionY = tmp_data[columnCount*i+offset+1];
        double positionZ = tmp_data[columnCount*i+offset+2];
        system->positions[3*i+0] = positionX;
        system->positions[3*i+1] = positionY;
        system->positions[3*i+2] = positionZ;
        system->initial_positions[3*i+0] = positionX;
        system->initial_positions[3*i+1] = positionY;
        system->initial_positions[3*i+2] = positionZ;
        offset += 3;

        system->velocities[3*i+0] = tmp_data[columnCount*i+offset+0];
        system->velocities[3*i+1] = tmp_data[columnCount*i+offset+1];
        system->velocities[3*i+2] = tmp_data[columnCount*i+offset+2];
        offset += 3;

    }
    system->count_frozen_atoms();
    delete tmp_data;
    //    system->mdtimer->end_io();
}

void MDIO::finalize() {
    if(movie_file_open) {
        movie_file->close();
    }

    fclose(energy_file);
    fclose(pressure_file);
    fclose(velocity_file);
    fclose(count_periodic_file);
}
