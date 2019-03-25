class ISA2 {
	int x = 0;
	int y = 0;
	int z = 0;
	int tmpy1 = 0;
	int tmpx2 = 0;
	int tmpz2 = 0;

	class Thread1 extends Thread {
		public void run(){
			x = 1;
			y = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmpy1 = y;
			z = 1;
		}
	}

	class Thread3 extends Thread {
		public void run(){
			tmpz2 = z;
			tmpx2 = x;
		}
	}


	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		Thread3 t3 = new Thread3();

		t1.start();
		t2.start();
		t3.start();

		try {
			t1.join();
			t2.join();
			t3.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			ISA2 isa = new ISA2();
			isa.test();
			if(isa.tmpy1 == 1 && isa.tmpz2 == 1 && isa.tmpx2 == 0) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM ISA2 Test PASSED after 100000 iterations");
		else
			System.out.println("ARM ISA2 Test FAILED: problem occurs on iteration " + counter);
	}
}
