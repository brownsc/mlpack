#include "hmm.h"
#include "multinomial.h"
#include "diag_gaussian.h"
#include "mixture.h"
#include "utils.h"


void TestMultinomial() {
  HMM<Multinomial> hmm;
  hmm.Init(2, 2, MULTINOMIAL);
  srand48(time(0));
  //hmm.RandomlyInitialize();
  
  ArrayList<GenMatrix<int> > sequences;
  sequences.Init(2);
  sequences[0].Init(1, 100);
  for(int i = 0; i < 100; i++) {
    sequences[0].set(0, i, i >= 50);
  }
  sequences[1].Init(1, 100);
  for(int i = 0; i < 100; i++) {
    sequences[1].set(0, i, i < 50);
  }

  printf("sequences[0]\n");
  for(int i = 0; i < 100; i++) {
    printf("%d ", sequences[0].get(0, i));
  }
  printf("\n");

  printf("sequences[1]\n");
  for(int i = 0; i < 100; i++) {
    printf("%d ", sequences[1].get(0, i));
  }
  printf("\n");


  hmm.InitParameters(sequences);

  hmm.PrintDebug("hmm after calling InitParameters(sequences)");

  hmm.ViterbiUpdate(sequences);

  hmm.PrintDebug("hmm after calling ViterbiUpdate(sequences)");
  
  hmm.BaumWelch(sequences,
		1e-10 * ((double)1),
		1000);
  
  hmm.PrintDebug("hmm after calling BaumWelch");
  /*
  hmm.PrintDebug("frozen HMM");
  

  const char* filename = "frozen_hmm";
  FILE* file = fopen(filename, "wb");
  index_t size = ot::FrozenSize(hmm);
  char* buf = mem::Alloc<char>(size);
  ot::Freeze(buf, hmm);
  fwrite(&size, sizeof(size), 1, file);
  fwrite(buf, 1, size, file);
  mem::Free(buf);
  fclose(file);
  
  HMM<Multinomial> thawed_hmm;
  FILE* file2 = fopen(filename, "rb");
  fread(&size, sizeof(size), 1, file2);
  char* buf2 = mem::Alloc<char>(size);
  fread(buf2, 1, size, file2);
  ot::InitThaw(&thawed_hmm, buf2);
  mem::Free(buf2);
  fclose(file2);

  thawed_hmm.PrintDebug("thawed HMM");
  



  ArrayList<HMM<Multinomial> > frozen_hmms;
  frozen_hmms.Init(2);
  for(int i = 0; i < 2; i++) {
    frozen_hmms[i].Init(2, 2, MULTINOMIAL);
    frozen_hmms[i].RandomlyInitialize();
  }

  for(int i = 0; i < 2; i++) {
    printf("frozen hmm %d\n", i);
    frozen_hmms[i].PrintDebug("");
  }

  WriteOutHMMArrayList("frozen_hmms", frozen_hmms);
  
  ArrayList<HMM<Multinomial> > thawed_hmms;
  ReadInHMMArrayList("frozen_hmms", &thawed_hmms);

  for(int i = 0; i < 2; i++) {
    printf("thawed hmm %d\n", i);
    thawed_hmms[i].PrintDebug("");
  }
  */

}


void TestGaussian() {
  HMM<DiagGaussian> hmm;
  hmm.Init(2, 2, GAUSSIAN, 0.0001);
  srand48(time(0));
  
  ArrayList<GenMatrix<double> > sequences;
  sequences.Init(2);
  sequences[0].Init(2, 100);
  for(int i = 0; i < 100; i++) {
    double num1 = drand48() / ((double)10) - 0.05;
    double num2 = drand48() / ((double)10) - 0.05;
    if(i >= 50) {
      num1 += ((double)1);
      num2 -= ((double)1);
    }
    sequences[0].set(0, i, num1);
    sequences[0].set(1, i, num2);
  }
  sequences[0].PrintDebug("sequences[0]");

  sequences[1].Init(2, 100);
  for(int i = 0; i < 100; i++) {
    double num1 = drand48() / ((double)10) - 0.05;
    double num2 = drand48() / ((double)10) - 0.05;
    if(i >= 50) {
      num1 += ((double)1);
      num2 -= ((double)1);
    }
    sequences[1].set(0, i, num1);
    sequences[1].set(1, i, num2);
  }
  sequences[1].PrintDebug("sequences[1]");

  hmm.InitParameters(sequences, false);

  hmm.PrintDebug("hmm after calling InitParameters(sequences)");

  hmm.ViterbiUpdate(sequences);

  hmm.PrintDebug("hmm after calling ViterbiUpdate(sequences)");
  
  hmm.BaumWelch(sequences,
		1e-10 * ((double)1),
		1000);

  hmm.PrintDebug("hmm after calling BaumWelch");
}


void TestMixture() {
  HMM<Mixture<DiagGaussian> > hmm;
  hmm.Init(4, 2, MIXTURE, 0.0001, 2);
  srand48(time(0));

  ArrayList<GenMatrix<double> > sequences;
  sequences.Init(1);
  for(int m = 0; m < 1; m++) {
    sequences[m].Init(2, 200);
    for(int i = 0; i < 200; i++) {
      double num1 = drand48() / ((double)10);
      double num2 = drand48() / ((double)10);
      if(i >= 50) {
	num1 += ((double)1) * (int)(i/50);
	num2 -= ((double)1) * (int)(i/50);
      }
      sequences[m].set(0, i, num1);
      sequences[m].set(1, i, num2);
    }
  }
  sequences[0].PrintDebug("sequences[0]");
  //sequences[1].PrintDebug("sequences[1]");

  hmm.InitParameters(sequences);

  hmm.PrintDebug("hmm after calling InitParameters(sequences)");

  hmm.ViterbiUpdate(sequences);

  hmm.PrintDebug("hmm after calling ViterbiUpdate(sequences)");

  hmm.BaumWelch(sequences,
		1e-6 * ((double)1),
		1000);

  hmm.PrintDebug("hmm after calling BaumWelch");

  Matrix p_x_given_q; // Rabiner's b

  ArrayList<Matrix> p_qq_t; // Rabiner's xi = P(q_t, q_{t+1} | X)
  Matrix p_qt; // Rabiner's gamma = P(q_t | X)
  double neg_likelihood = 1;
  
  printf("hmm.ExpectationStepNoLearning()\n");
  hmm.ExpectationStepNoLearning(sequences[0],
				&p_x_given_q,
				&p_qq_t,
				&p_qt,
				&neg_likelihood);
  /*
    p_x_given_q.PrintDebug("p_x_given_q");
    for(int i = 0; i < p_qq_t.size(); i++) {
    printf("state %d\n", i);
    p_qq_t[i].PrintDebug("p_qq_t");
    }
    p_qt.PrintDebug("p_qt");
  */
  printf("neg_likelihood = %f\n", neg_likelihood);
  
  
}


void TestGaussianPdf() {
  DiagGaussian g;
  g.Init(2, 0.001);
  Vector &mu = *(g.mu_);
  mu[0] = -1;
  mu[1] = 0.5;

  Vector &sigma = *(g.sigma_);
  sigma[0] = 0.5;
  sigma[1] = 0.4;
  
  g.ComputeNormConstant();
  g.PrintDebug("g");
  
  Vector x;
  x.Init(2);
  x[0] = 2;
  x[1] = 1;
  printf("%e\n", g.Pdf(x));


}


int main(int argc, char* argv[]) {
  //TestGaussianPdf();

  //TestMultinomial();
  TestGaussian();
  //TestMixture();
}
