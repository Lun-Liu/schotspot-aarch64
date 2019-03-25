class SB1Array {
	int[] x = {0,0};
	int[] tmp = {0,0};

	class Thread1 extends Thread {
		public void run(){
			x[0] = 1;
			tmp[1] = x[1];
		}
	}

	class Thread2 extends Thread {
		public void run(){
			x[1] = 1;
			tmp[0] = x[0];
		}
	}

	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		t1.start();
		t2.start();

		try {
			t1.join();
			t2.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			SB1Array sb = new SB1Array();
			sb.test();
			if(sb.tmp[0] == 0 && sb.tmp[1] == 0)
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM SB Test PASSED after 100000 iterations");
		else
			System.out.println("ARM SB Test FAILED: problem occurs on iteration " + counter);
	}
}
