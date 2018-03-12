
#include <WarpX.H>
#include <WarpX_f.H>

using namespace amrex;

void
WarpX::AllocLevelDataFFT (int lev)
{
    AMREX_ALWAYS_ASSERT_WITH_MESSAGE(lev == 0, "PSATD doesn't work with mesh refinement yet");

    static_assert(std::is_standard_layout<FFTData>::value, "FFTData must have standard layout");

    InitFFTComm(lev);

    BoxArray ba_fp_fft;
    DistributionMapping dm_fp_fft;
    FFTDomainDecompsition(lev, ba_fp_fft, dm_fp_fft, ba_valid_fp_fft[lev], geom[lev].Domain());

    int ngRho = Efield_fp[lev][0]->nGrow();
    if (rho_fp[lev] == nullptr)
    {
        const BoxArray& ba = Efield_fp[lev][0]->boxArray();
        const DistributionMapping& dm = Efield_fp[lev][0]->DistributionMap();
        rho_fp[lev].reset(new MultiFab(amrex::convert(ba,IntVect::TheUnitVector()),dm,1,ngRho));
    }

    rho2_fp[lev].reset(new MultiFab(rho_fp[lev]->boxArray(),
                                    rho_fp[lev]->DistributionMap(),
                                    1, ngRho));

    Efield_fp_fft[lev][0].reset(new MultiFab(amrex::convert(ba_fp_fft,Ex_nodal_flag),
                                             dm_fp_fft, 1, 0));
    Efield_fp_fft[lev][1].reset(new MultiFab(amrex::convert(ba_fp_fft,Ey_nodal_flag),
                                             dm_fp_fft, 1, 0));
    Efield_fp_fft[lev][2].reset(new MultiFab(amrex::convert(ba_fp_fft,Ez_nodal_flag),
                                             dm_fp_fft, 1, 0));
    Bfield_fp_fft[lev][0].reset(new MultiFab(amrex::convert(ba_fp_fft,Bx_nodal_flag),
                                             dm_fp_fft, 1, 0));
    Bfield_fp_fft[lev][1].reset(new MultiFab(amrex::convert(ba_fp_fft,By_nodal_flag),
                                             dm_fp_fft, 1, 0));
    Bfield_fp_fft[lev][2].reset(new MultiFab(amrex::convert(ba_fp_fft,Bz_nodal_flag),
                                             dm_fp_fft, 1, 0));
    current_fp_fft[lev][0].reset(new MultiFab(amrex::convert(ba_fp_fft,jx_nodal_flag),
                                              dm_fp_fft, 1, 0));
    current_fp_fft[lev][1].reset(new MultiFab(amrex::convert(ba_fp_fft,jy_nodal_flag),
                                              dm_fp_fft, 1, 0));
    current_fp_fft[lev][2].reset(new MultiFab(amrex::convert(ba_fp_fft,jz_nodal_flag),
                                              dm_fp_fft, 1, 0));
    rho_prev_fp_fft[lev].reset(new MultiFab(amrex::convert(ba_fp_fft,IntVect::TheNodeVector()),
                                            dm_fp_fft, 1, 0));
    rho_next_fp_fft[lev].reset(new MultiFab(amrex::convert(ba_fp_fft,IntVect::TheNodeVector()),
                                            dm_fp_fft, 1, 0));

    dataptr_fp_fft[lev].reset(new LayoutData<FFTData>(ba_fp_fft, dm_fp_fft));

    if (lev > 0)
    {
        BoxArray ba_cp_fft;
        DistributionMapping dm_cp_fft;
        FFTDomainDecompsition(lev, ba_cp_fft, dm_cp_fft, ba_valid_cp_fft[lev],
                              amrex::coarsen(geom[lev].Domain(),2));
        
        int ngRho = Efield_cp[lev][0]->nGrow();
        if (rho_cp[lev] == nullptr)
        {
            const BoxArray& ba = Efield_cp[lev][0]->boxArray();
            const DistributionMapping& dm = Efield_cp[lev][0]->DistributionMap();
            rho_cp[lev].reset(new MultiFab(amrex::convert(ba,IntVect::TheUnitVector()),dm,1,ngRho));
        }
        
        rho2_cp[lev].reset(new MultiFab(rho_cp[lev]->boxArray(),
                                        rho_cp[lev]->DistributionMap(),
                                        1, ngRho));

        Efield_cp_fft[lev][0].reset(new MultiFab(amrex::convert(ba_cp_fft,Ex_nodal_flag),
                                                 dm_cp_fft, 1, 0));
        Efield_cp_fft[lev][1].reset(new MultiFab(amrex::convert(ba_cp_fft,Ey_nodal_flag),
                                                 dm_cp_fft, 1, 0));
        Efield_cp_fft[lev][2].reset(new MultiFab(amrex::convert(ba_cp_fft,Ez_nodal_flag),
                                                 dm_cp_fft, 1, 0));
        Bfield_cp_fft[lev][0].reset(new MultiFab(amrex::convert(ba_cp_fft,Bx_nodal_flag),
                                                 dm_cp_fft, 1, 0));
        Bfield_cp_fft[lev][1].reset(new MultiFab(amrex::convert(ba_cp_fft,By_nodal_flag),
                                                 dm_cp_fft, 1, 0));
        Bfield_cp_fft[lev][2].reset(new MultiFab(amrex::convert(ba_cp_fft,Bz_nodal_flag),
                                                 dm_cp_fft, 1, 0));
        current_cp_fft[lev][0].reset(new MultiFab(amrex::convert(ba_cp_fft,jx_nodal_flag),
                                                  dm_cp_fft, 1, 0));
        current_cp_fft[lev][1].reset(new MultiFab(amrex::convert(ba_cp_fft,jy_nodal_flag),
                                                  dm_cp_fft, 1, 0));
        current_cp_fft[lev][2].reset(new MultiFab(amrex::convert(ba_cp_fft,jz_nodal_flag),
                                                  dm_cp_fft, 1, 0));
        rho_prev_cp_fft[lev].reset(new MultiFab(amrex::convert(ba_cp_fft,IntVect::TheNodeVector()),
                                                dm_cp_fft, 1, 0));
        rho_next_cp_fft[lev].reset(new MultiFab(amrex::convert(ba_cp_fft,IntVect::TheNodeVector()),
                                                dm_cp_fft, 1, 0));
        
        dataptr_cp_fft[lev].reset(new LayoutData<FFTData>(ba_cp_fft, dm_cp_fft));
    }
}

void
WarpX::InitFFTComm (int lev)
{
    int nprocs = ParallelDescriptor::NProcs();
    ngroups_fft = std::min(ngroups_fft, nprocs);

    // # of processes in the subcommunicator
    int np_fft = nprocs / ngroups_fft;
    AMREX_ALWAYS_ASSERT_WITH_MESSAGE(np_fft*ngroups_fft == nprocs,
                                     "Number of processes must be divisilbe by number of FFT groups");

    int myproc = ParallelDescriptor::MyProc();
    // my color in ngroups_fft subcommunicators.  0 <= color_fft < ngroups_fft
    color_fft[lev] = myproc / np_fft;
    MPI_Comm_split(ParallelDescriptor::Communicator(), color_fft[lev], myproc, &comm_fft[lev]);

    int fcomm = MPI_Comm_c2f(comm_fft[lev]);
    warpx_fft_mpi_init(fcomm);
}

void
WarpX::FFTDomainDecompsition (int lev, amrex::BoxArray& ba_fft, amrex::DistributionMapping& dm_fft,
                              amrex::BoxArray& ba_valid, const Box& domain)
{
    int nprocs = ParallelDescriptor::NProcs();
    int np_fft;
    MPI_Comm_size(comm_fft[lev], &np_fft);

    BoxList bl_fft;
    bl_fft.reserve(nprocs);
    Vector<int> gid_fft;
    gid_fft.reserve(nprocs);
    
    Box domain_fft; // the "global" domain for a group
    BoxList bl(domain, ngroups_fft);  // This does a multi-D domain decomposition for groups
    AMREX_ALWAYS_ASSERT(bl.size() == ngroups_fft);
    const Vector<Box>& bldata = bl.data();
    for (int igroup = 0; igroup < ngroups_fft; ++igroup)
    {
        // Within the group, 1d domain decomposition is performed.
        const Box& bx = amrex::grow(bldata[igroup], nguards_fft);
        // chop in z-direction into np_fft for FFTW
        BoxList tbl(bx, np_fft, Direction::z);
        bl_fft.join(tbl);
        for (int i = 0; i < np_fft; ++i) {
            gid_fft.push_back(igroup);
        }
        if (igroup == color_fft[lev]) {
            domain_fft = bx;
        }
    }

    // This BoxArray contains local FFT domains for each process
    ba_fft.define(std::move(bl_fft));
    AMREX_ALWAYS_ASSERT(ba_fft.size() == ParallelDescriptor::NProcs());

    Vector<int> pmap(ba_fft.size());
    std::iota(pmap.begin(), pmap.end(), 0);
    dm_fft.define(std::move(pmap));

    //
    // For communication between WarpX normal domain and FFT domain, we need to create a 
    // special BoxArray ba_valid
    //

    const Box foobox(IntVect(AMREX_D_DECL(-nguards_fft-2,-nguards_fft-2,-nguards_fft-2)),
                     IntVect(AMREX_D_DECL(-nguards_fft-2,-nguards_fft-2,-nguards_fft-2)));

    BoxList bl_valid;
    bl_valid.reserve(ba_fft.size());
    for (int i = 0; i < ba_fft.size(); ++i)
    {
        int igroup = gid_fft[i];
        const Box& bx = ba_fft[i] & bldata[igroup];
        if (bx.ok())
        {
            bl_valid.push_back(bx);
        }
        else
        {
            bl_valid.push_back(foobox);
        }
    }

    ba_valid.define(std::move(bl_valid));
}

void
WarpX::FreeFFTCommPlan (int lev)
{
    if (comm_fft[lev] != MPI_COMM_NULL) {
        MPI_Comm_free(&comm_fft[lev]);
    }
    comm_fft[lev] = MPI_COMM_NULL;
}
